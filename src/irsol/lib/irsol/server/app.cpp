#include "irsol/server/app.hpp"

#include "irsol/logging.hpp"
#include "irsol/server/handlers.hpp"
#include "irsol/utils.hpp"

#include <sstream>

namespace irsol {
namespace server {

App::App(irsol::types::port_t port)
  : m_port(port)
  , m_running(false)
  , m_acceptor({})
  , m_cameraInterface(std::make_unique<camera::Interface>(camera::Interface::HalfResolution()))
  , m_frameCollector(std::make_unique<internal::FrameCollector>(*m_cameraInterface.get()))
  , m_messageHandler(std::make_unique<handlers::MessageHandler>())
{
  registerMessageHandlers();
  IRSOL_LOG_DEBUG("App created on port {}", m_port);
}

bool
App::start()
{
  IRSOL_LOG_INFO("Starting server on port {}", m_port);
  if(auto openResult = m_acceptor.open(irsol::types::inet_address_t(m_port)); !openResult) {
    IRSOL_LOG_ERROR("Failed to open acceptor on port {}: {}", m_port, openResult.error().message());
    return false;
  }
  m_running = true;
  IRSOL_LOG_DEBUG("Starting accept thread");
  m_acceptThread = std::thread(&App::acceptLoop, this);
  IRSOL_LOG_INFO("Server started successfully");
  return true;
}

void
App::stop()
{
  IRSOL_LOG_INFO("Stopping server");
  m_running = false;
  m_acceptor.close();
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
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  auto                        it = m_clients.find(clientId);
  return it != m_clients.end() ? it->second : nullptr;
}

void
App::acceptLoop()
{
  IRSOL_LOG_INFO("Accept loop started");
  while(m_running) {

    // Set non-blocking mode to avoid blocking indefinitely on accept
    m_acceptor.set_non_blocking(true);

    auto sockResult = m_acceptor.accept();

    // Check if we should exit the loop
    if(!m_running) {
      IRSOL_LOG_DEBUG("Accept loop stopped");
      break;
    }

    if(!sockResult) {
      if(m_running) {
        sockpp::error_code err{sockResult.error()};
        // These errors are expected in non-blocking mode when no connection is available
        bool isExpectedError =
          (err == std::errc::resource_unavailable_try_again ||
           err == std::errc::operation_would_block || err == std::errc::timed_out);

        if(isExpectedError) {
          // Sleep for a short time to avoid busy waiting when no connections are available
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
          // Log unexpected errors
          IRSOL_LOG_WARN("Failed to accept connection: {}", sockResult.error_message());
        }
      }
      continue;
    }

    // Generate a unique ID for this client session
    irsol::types::client_id_t clientId = utils::uuid();
    IRSOL_LOG_DEBUG("Generated client ID: {}", clientId);
    auto session = std::make_shared<internal::ClientSession>(clientId, sockResult.release(), *this);
    addClient(clientId, session);
  }
  IRSOL_LOG_INFO("Accept loop ended");
}

void
App::addClient(
  const irsol::types::client_id_t&         clientId,
  std::shared_ptr<internal::ClientSession> session)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  IRSOL_LOG_INFO("New client connection from {}", session->socket().address().to_string());
  {
    m_clients.insert({clientId, session});
    IRSOL_LOG_DEBUG(
      "Client {} added to session list, total clients: {}", clientId, m_clients.size());
  }

  std::thread([session, this]() {
    std::string clientAddress = session->socket().address().to_string();
    IRSOL_LOG_DEBUG(
      "Starting client session thread for {} with ID {}", clientAddress, session->id());

    try {
      session->run();
    } catch(std::exception& e) {
      IRSOL_LOG_ERROR("Error in client session thread for {}: {}", clientAddress, e.what());
    }
    removeClient(session->id());
  })
    .detach();
}

void
App::removeClient(const irsol::types::client_id_t& clientId)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  auto                        client = m_clients.find(clientId);
  if(client == m_clients.end()) {
    IRSOL_LOG_ERROR("Client '{}' not found in session list", clientId);
  } else {
    m_clients.erase(client);
    m_frameCollector->removeClient(client->second);
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
  registerMessageHandler<protocol::Inquiry, handlers::InquiryFRHandler>("fr", ctx);
  registerMessageHandler<protocol::Command, handlers::CommandGIHandler>("gi", ctx);
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
    [](
      handlers::Context&                 ctx,
      const ::irsol::types::client_id_t& client_id,
      protocol::Command&&                cmd) -> std::vector<protocol::OutMessage> {
      std::vector<protocol::OutMessage> result;
      auto&                             cam = ctx.app.camera();
      auto                              img = cam.captureImage(std::chrono::milliseconds(10000));

      if(img.IsEmpty()) {
        IRSOL_NAMED_LOG_ERROR(client_id, "Failed to capture image.");
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

  // Resister all handlers
  // if(!m_messageHandler->registerHandler<protocol::Inquiry>("fr",
  // handlers::InquiryFRHandler(ctx))) {
  //   IRSOL_LOG_FATAL("Failed to register inquiry frame rate handler");
  //   throw std::runtime_error("Failed to register inquiry frame rate handler");
  // };
  // if(!m_messageHandler->registerHandler<protocol::Command>("gi",
  // handlers::CommandGIHandler(ctx))) {
  //   IRSOL_LOG_FATAL("Failed to register get image handler");
  //   throw std::runtime_error("Failed to register get image handler");
  // }
  // if(!m_messageHandler->registerHandler<protocol::Command>(
  //      "image_data",
  //      handlers::CommandLambdaHandler(
  //        ctx,
  //        [](
  //          handlers::Context&                  ctx,
  //          const ::irsol::types::client_id_t& client_id,
  //          protocol::Command&&                 cmd) -> std::vector<protocol::OutMessage> {
  //          std::vector<protocol::OutMessage> result;
  //          auto&                             cam = ctx.app.camera();
  //          auto img = cam.captureImage(std::chrono::milliseconds(10000));
  //          if(img.IsEmpty()) {
  //            IRSOL_NAMED_LOG_ERROR(client_id, "Failed to capture image.");
  //            result.emplace_back(irsol::protocol::Error::from(cmd, "Failed to capture image"));
  //            return result;
  //          }
  //          uint32_t width    = static_cast<uint32_t>(img.GetWidth());
  //          uint32_t height   = static_cast<uint32_t>(img.GetHeight());
  //          size_t   dataSize = img.GetSize();

  //          // Send raw data
  //          const void*                             imageBuffer = img.GetImageData();
  //          std::vector<irsol::types::byte_t> rawData(dataSize);
  //          memcpy(rawData.data(), imageBuffer, dataSize);

  //          result.emplace_back(
  //            irsol::protocol::ImageBinaryData(std::move(rawData), {height, width}, {}));
  //          return result;
  //        }))) {
  //   IRSOL_ASSERT_DEBUG("Failed to register image handler");
  //   throw std::runtime_error("Failed to register image handler");
  // }
}

}  // namespace server
}  // namespace irsol
