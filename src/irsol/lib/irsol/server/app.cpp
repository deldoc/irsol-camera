// ServerApp.cpp
#include "irsol/server/app.hpp"
#include "irsol/logging.hpp"

namespace irsol {
ServerApp::ServerApp(int port)
    : m_port(port), m_running(false), m_acceptor({}), m_cameraController() {
  IRSOL_LOG_DEBUG("ServerApp created on port {}", m_port);
}

bool ServerApp::start() {
  IRSOL_LOG_INFO("Starting server on port {}", m_port);
  if (auto openResult = m_acceptor.open(sockpp::inet_address(m_port)); !openResult) {
    IRSOL_LOG_ERROR("Failed to open acceptor on port {}: {}", m_port, openResult.error().message());
    return false;
  }
  m_running = true;
  IRSOL_LOG_DEBUG("Starting accept thread");
  m_acceptThread = std::thread(&ServerApp::acceptLoop, this);
  IRSOL_LOG_INFO("Server started successfully");
  return true;
}

void ServerApp::stop() {
  IRSOL_LOG_INFO("Stopping server");
  m_running = false;
  m_acceptor.close();
  if (m_acceptThread.joinable()) {
    IRSOL_LOG_DEBUG("Joining accept thread");
    m_acceptThread.join();
  }
  IRSOL_LOG_INFO("Server stopped");
}

void ServerApp::acceptLoop() {
  IRSOL_LOG_INFO("Accept loop started");
  while (m_running) {

    // Set non-blocking mode to avoid blocking indefinitely on accept
    m_acceptor.set_non_blocking(true);

    auto sockResult = m_acceptor.accept();

    // Check if we should exit the loop
    if (!m_running) {
      IRSOL_LOG_DEBUG("Accept loop stopped");
      break;
    }

    if (!sockResult) {
      if (m_running) {
        auto err = sockResult.error();
        // These errors are expected in non-blocking mode when no connection is available
        bool isExpectedError =
            (err == std::errc::resource_unavailable_try_again ||
             err == std::errc::operation_would_block || err == std::errc::timed_out);

        if (isExpectedError) {
          IRSOL_LOG_TRACE("No new connection available '{}'. Sleeping for a short time.",
                          err.message());
          // Sleep for a short time to avoid busy waiting when no connections are available
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
          // Log unexpected errors
          IRSOL_LOG_WARN("Failed to accept connection: {}", sockResult.error_message());
        }
      }
      continue;
    }

    // Accept the new client connection and push it into a new handler thread.
    auto sock = sockResult.release();
    auto clientAddress = sock.address().to_string();
    IRSOL_LOG_INFO("New client connection from {}", clientAddress);

    auto session = std::make_shared<ClientSession>(std::move(sock), *this);
    {
      std::lock_guard<std::mutex> lock(m_clientsMutex);
      m_clients.insert(session);
      IRSOL_LOG_DEBUG("Client added to session list, total clients: {}", m_clients.size());
    }

    std::thread([session, clientAddress, this]() {
      IRSOL_LOG_DEBUG("Starting client session thread for {}", clientAddress);
      session->run();
      // Once the session is done, remove it from the list and join the thread.
      removeClient(session);
    })
        .detach();
  }
  IRSOL_LOG_INFO("Accept loop ended");
}

void ServerApp::removeClient(const std::shared_ptr<ClientSession> &client) {
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  m_clients.erase(client);
  IRSOL_LOG_DEBUG("Client removed from session list, remaining clients: {}", m_clients.size());
}

void ServerApp::broadcast(const std::string &msg) {
  IRSOL_LOG_DEBUG("Broadcasting message to {} clients", m_clients.size());
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  for (auto &client : m_clients) {
    client->send(msg);
  }
}

CameraController &ServerApp::camera() { return m_cameraController; }
} // namespace irsol
