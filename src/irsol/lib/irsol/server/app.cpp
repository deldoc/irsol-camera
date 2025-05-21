// App.cpp
#include "irsol/server/app.hpp"

#include "irsol/logging.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {

App::App(port_t port)
  : m_port(port)
  , m_running(false)
  , m_acceptor({})
  , m_cameraInterface(std::make_unique<camera::Interface>())
  , m_frameCollector(std::make_unique<internal::FrameCollector>(*m_cameraInterface.get()))
{
  IRSOL_LOG_DEBUG("App created on port {}", m_port);
}

bool
App::start()
{
  IRSOL_LOG_INFO("Starting server on port {}", m_port);
  if(auto openResult = m_acceptor.open(inet_address_t(m_port)); !openResult) {
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
        auto err = sockResult.error();
        // These errors are expected in non-blocking mode when no connection is available
        bool isExpectedError =
          (err == std::errc::resource_unavailable_try_again ||
           err == std::errc::operation_would_block || err == std::errc::timed_out);

        if(isExpectedError) {
          IRSOL_LOG_TRACE(
            "No new connection available '{}'. Sleeping for a short time.", err.message());
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
    client_id_t clientId = utils::uuid();
    IRSOL_LOG_DEBUG("Generated client ID: {}", clientId);
    auto session = std::make_shared<internal::ClientSession>(clientId, sockResult.release(), *this);
    addClient(clientId, session);
  }
  IRSOL_LOG_INFO("Accept loop ended");
}

void
App::addClient(const client_id_t& clientId, std::shared_ptr<internal::ClientSession> session)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  IRSOL_LOG_INFO(
    "New client connection from {}", session->sessionData().sock.address().to_string());
  {
    m_clients.insert({clientId, session});
    IRSOL_LOG_DEBUG(
      "Client {} added to session list, total clients: {}", clientId, m_clients.size());
  }

  std::thread([session, this]() {
    std::string clientAddress = session->sessionData().sock.address().to_string();
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
App::removeClient(const client_id_t& clientId)
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
App::broadcast(const std::string& msg)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  IRSOL_LOG_DEBUG("Broadcasting message '{}' to {} clients", msg, m_clients.size());
  for(auto& [clientId, client] : m_clients) {
    IRSOL_LOG_TRACE("Broadcasting message '{}' to client '{}'", msg, clientId);
    // Lock the client's session to prevent race conditions
    std::lock_guard<std::mutex> clientLock(client->sessionData().mutex);
    client->send(msg);
  }
  IRSOL_LOG_DEBUG("Broadcasted message '{}' to {} clients", msg, m_clients.size());
}

camera::Interface&
App::camera()
{
  return *m_cameraInterface;
}
internal::FrameCollector&
App::frameCollector()
{
  return *m_frameCollector;
}

}  // namespace server
}  // namespace irsol
