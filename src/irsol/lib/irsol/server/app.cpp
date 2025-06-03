#include "irsol/server/app.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol/serialization.hpp"
#include "irsol/server/handlers.hpp"
#include "irsol/utils.hpp"

#include <sstream>

namespace irsol {
namespace server {

App::App(irsol::types::port_t port)
  : m_port(port)
  , m_acceptor(
      m_port,
      std::bind(&App::addClient, this, std::placeholders::_1, std::placeholders::_2))
  , m_cameraInterface(std::make_unique<camera::Interface>(camera::Interface::HalfResolution()))
  , m_frameCollector(std::make_unique<frame_collector::FrameCollector>(*m_cameraInterface.get()))
  , m_messageHandler(std::make_unique<handlers::MessageHandler>())
{
  registerMessageHandlers();
  IRSOL_LOG_DEBUG("App ready to run on port {}", m_port);
}

bool
App::start()
{
  if(!m_acceptor.isOpen()) {
    IRSOL_LOG_ERROR("Failed to open acceptor on port {}: {}", m_port, m_acceptor.error());
    return false;
  }
  IRSOL_LOG_DEBUG("Starting accept thread");
  m_acceptThread = std::thread(&internal::ClientSessionAcceptor::run, &m_acceptor);
  IRSOL_LOG_INFO("Server started successfully");
  return true;
}

void
App::stop()
{
  IRSOL_LOG_INFO("Stopping server");
  m_acceptor.stop();

  if(m_acceptThread.joinable()) {
    IRSOL_LOG_DEBUG("Joining accept thread");
    m_acceptThread.join();
  }
  m_frameCollector->stop();
  IRSOL_LOG_INFO("Server stopped");
}

std::shared_ptr<internal::ClientSession>
App::getClientSession(const irsol::types::client_id_t& clientId)
{
  std::scoped_lock<std::mutex> lock(m_clientsMutex);
  auto                         it = m_clients.find(clientId);
  return it != m_clients.end() ? it->second : nullptr;
}

void
App::broadcastMessage(
  protocol::OutMessage&&                          message,
  const std::optional<irsol::types::client_id_t>& excludeClient)
{
  // Serialize the message only once, and distribute to all clients
  auto serializedMessage = irsol::protocol::Serializer::serialize(std::move(message));
  IRSOL_LOG_DEBUG("Broadcasting serialized message {}", serializedMessage.toString());
  std::scoped_lock<std::mutex> lock(m_clientsMutex);

  for(const auto& [clientId, session] : m_clients) {
    if(excludeClient && clientId == *excludeClient) {
      continue;  // Skip the sender if specified
    }
    try {
      std::scoped_lock<std::mutex> sessionLock(session->socketMutex());
      session->handleSerializedMessage(serializedMessage);
    } catch(const std::exception& ex) {
      IRSOL_LOG_WARN("Failed to send broadcast to client {}: {}", clientId, ex.what());
    }
  }
  IRSOL_LOG_DEBUG("Broadcasting complete.");
}

void
App::addClient(const irsol::types::client_id_t& clientId, irsol::types::socket_t&& sock)
{
  auto session = std::make_shared<internal::ClientSession>(clientId, std::move(sock), *this);
  std::scoped_lock<std::mutex> lock(m_clientsMutex);
  IRSOL_LOG_INFO(
    "Registering new client connection from {} with id {}",
    session->socket().address().to_string(),
    clientId);
  {
    m_clients.insert({clientId, session});
    IRSOL_LOG_DEBUG(
      "Client {} added to session list, total clients: {}", clientId, m_clients.size());
  }

  IRSOL_LOG_INFO("Starting client session thread for client with id {}", clientId);
  std::thread([session, this]() {
    IRSOL_LOG_DEBUG("Thread for client with ID {} started", session->id());

    try {
      session->run();
    } catch(std::exception& e) {
      IRSOL_LOG_ERROR(
        "Error in client session thread for client with id {}: {}", session->id(), e.what());
    }
    IRSOL_LOG_DEBUG("Thread for client with ID {} finished", session->id());
    removeClient(session->id());
  })
    .detach();
}

void
App::removeClient(const irsol::types::client_id_t& clientId)
{
  std::scoped_lock<std::mutex> lock(m_clientsMutex);
  auto                         clientIt = m_clients.find(clientId);
  if(clientIt == m_clients.end()) {
    IRSOL_LOG_ERROR("Client '{}' not found in session list", clientId);
  } else {
    auto client = clientIt->second;
    m_frameCollector->deregisterClient(client->id());
    m_clients.erase(clientIt);
    IRSOL_LOG_DEBUG(
      "Client {} removed from session list, remaining clients: {}", clientId, m_clients.size());
  }
}

void
App::registerMessageHandlers()
{
  // Build a context to pass to all handlers
  handlers::Context ctx{*this};

  // Register message handlers for specific message types
  registerMessageHandler<protocol::Inquiry, handlers::InquiryFrameRateHandler>("fr", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentFrameRateHandler>("fr", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentInputSequenceLengthHandler>(
    "isl", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentIntegrationTimeHandler>(
    "it", ctx);
  registerMessageHandler<protocol::Inquiry, handlers::InquiryIntegrationTimeHandler>("it", ctx);
  registerMessageHandler<protocol::Inquiry, handlers::InquiryInputSequenceLengthHandler>(
    "isl", ctx);
  registerMessageHandler<protocol::Command, handlers::CommandAbortHandler>("abort", ctx);
  registerMessageHandler<protocol::Command, handlers::CommandGIHandler>("gi", ctx);
  registerMessageHandler<protocol::Command, handlers::CommandGISHandler>("gis", ctx);
  registerMessageHandler<protocol::Inquiry, handlers::InquiryImgLeftHandler>("img_l", ctx);
  registerMessageHandler<protocol::Inquiry, handlers::InquiryImgTopHandler>("img_t", ctx);
  registerMessageHandler<protocol::Inquiry, handlers::InquiryImgWidthHandler>("img_w", ctx);
  registerMessageHandler<protocol::Inquiry, handlers::InquiryImgHeightHandler>("img_h", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentImgLeftHandler>("img_l", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentImgTopHandler>("img_t", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentImgWidthHandler>("img_w", ctx);
  registerMessageHandler<protocol::Assignment, handlers::AssignmentImgHeightHandler>("img_h", ctx);

  registerLambdaHandler<protocol::Command>(
    "image_data",
    ctx,
    [](handlers::Context& ctx, const irsol::types::client_id_t& clientId, protocol::Command&& cmd)
      -> std::vector<protocol::OutMessage> {
      std::vector<protocol::OutMessage> result;
      auto&                             cam = ctx.app.camera();
      auto                              img = cam.captureImage(std::chrono::milliseconds(10000));

      if(img.IsEmpty()) {
        IRSOL_NAMED_LOG_ERROR(clientId, "Failed to capture image.");
        result.emplace_back(irsol::protocol::Error::from(cmd, "Failed to capture image"));
        return result;
      }

      uint32_t width    = static_cast<uint32_t>(img.GetWidth());
      uint32_t height   = static_cast<uint32_t>(img.GetHeight());
      size_t   dataSize = img.GetSize();

      const void*                       imageBuffer = img.GetImageData();
      std::vector<irsol::types::byte_t> rawData(dataSize);
      memcpy(rawData.data(), imageBuffer, dataSize);

      result.emplace_back(
        irsol::protocol::ImageBinaryData(std::move(rawData), {height, width}, {}));
      return result;
    });
}

}  // namespace server
}  // namespace irsol
