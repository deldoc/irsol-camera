#include "irsol/server/server.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/server/command_handler.hpp"

#include <cstring>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace irsol {
namespace server {
Server::Server(uint16_t port, uint16_t maxClients, int16_t stdInputCommunicationId)
    : m_port(port), m_maxClients(maxClients), m_stdInputCommunicationId(stdInputCommunicationId),
      m_terminate(false), m_fds(maxClients) {
  IRSOL_LOG_INFO("Initializing server on port {0:d} with {1:d} max clients.", m_port, m_maxClients);
  for (auto &fd : m_fds) {
    fd.fd = -1;
  }
  if (m_stdInputCommunicationId > -1) {
    // Make sure the id is not larger than the max number of clients.
    IRSOL_ASSERT_ERROR(m_stdInputCommunicationId < m_maxClients,
                       "stdInputCommunicationId (%d) is larger or equal to the number of max "
                       "clients (%d). This is not allowed.",
                       m_stdInputCommunicationId, m_maxClients);
    IRSOL_LOG_INFO("Allowing standard input communication via id {0:d}", m_stdInputCommunicationId);
  }
  initServer();
}

Server::~Server() {
  close(m_serverSocketId);
  IRSOL_LOG_INFO("Server shut down.");
}

void Server::initServer() {
  IRSOL_LOG_INFO("Initializing server..");
  m_serverSocketId = socket(internal::ADDRESS_FAMILY, internal::SOCKET_TYPE, 0);
  if (m_serverSocketId < 0) {
    IRSOL_LOG_FATAL("Failed to create socket: {}", strerror(errno));
    throw std::runtime_error("Socket creation failed");
  }

  IRSOL_LOG_DEBUG("Binding the server socket {0:d} to port {1:d}.", m_serverSocketId, m_port);
  sockaddr_in serverAddr{};
  serverAddr.sin_family = internal::ADDRESS_FAMILY;
  serverAddr.sin_port = htons(m_port);
  serverAddr.sin_addr.s_addr = internal::IN_ADDRESS_ANY;

  if (bind(m_serverSocketId, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    IRSOL_LOG_FATAL("Failed to bind socket: {}", strerror(errno));
    throw std::runtime_error("Socket bind failed");
  }

  IRSOL_LOG_DEBUG("Registering the server socket {0:d} in poll list.", m_serverSocketId);
  m_fds[internal::SERVER_FD_IDX].fd = m_serverSocketId;
  m_fds[internal::SERVER_FD_IDX].events = POLLIN | POLLHUP;
  m_fds[internal::SERVER_FD_IDX].revents = 0;

  if (m_stdInputCommunicationId > -1) {
    IRSOL_LOG_DEBUG("Register std input socket {0:d} in poll list.", m_stdInputCommunicationId);
    m_fds[internal::STDIN_FD_IDX].fd = 0;
    m_fds[internal::STDIN_FD_IDX].events = POLLIN;
  }

  IRSOL_LOG_INFO("Server initialized successfully on port {0:d}", m_port);
}

void Server::run() {
  IRSOL_LOG_INFO("Server starting event loop...");
  while (!m_terminate) {
    // TODO: implement timer mechanism for
    // not polling too often.
    int pollCount = poll(m_fds.data(), m_maxClients, 1000);
    if (pollCount < 0) {
      IRSOL_LOG_ERROR("Poll error: {}", strerror(errno));
      continue;
    }

    IRSOL_LOG_INFO("Processing {0:d} I/O events.", pollCount);
    for (size_t i = 0; i < m_maxClients; ++i) {
      auto &fd = m_fds[i];
      if (m_terminate) {
        // Early break in case m_terminate flag is set
        // externally.
        break;
      }
      if (fd.fd >= 0 && (fd.revents & POLLNVAL)) {
        IRSOL_LOG_WARN("Poll returns invalid polling request fd={0:d} events={1:d} revents={2:d}",
                       fd.fd, fd.events, fd.revents);
        close(fd.fd);
        fd.fd = -1;
        fd.events = 0;
      } else if (fd.fd >= 0 && (fd.events & fd.revents)) {
        if (i == internal::SERVER_FD_IDX) {
          // TODO: fix
          // process_connection_request(fd);
        } else {
          // TODO: fix
          // serve_channel(i, 0);
        }
      }

      fd.revents = 0;
    }
  }
  IRSOL_LOG_INFO("Server stopped event loop.");
  close(m_serverSocketId);
}

void Server::terminate() {
  IRSOL_LOG_INFO("Server is stopping...");
  m_terminate = true;
}

void Server::acceptConnection() {
  sockaddr_in clientAddr{};
  socklen_t clientLen = sizeof(clientAddr);
  int clientSocket = accept(m_serverSocketId, (struct sockaddr *)&clientAddr, &clientLen);

  if (clientSocket < 0) {
    IRSOL_LOG_WARN("Failed to accept connection: {}", strerror(errno));
    return;
  }

  IRSOL_LOG_INFO("Accepted new connection: socket {}", clientSocket);
  for (auto &fd : m_fds) {
    if (fd.fd == 0) {
      fd.fd = clientSocket;
      fd.events = POLLIN;
      break;
    }
  }
}

void Server::handleClient(int clientSocket) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);

  if (bytesRead <= 0) {
    IRSOL_LOG_INFO("Client disconnected: socket {}", clientSocket);
    close(clientSocket);
    return;
  }

  IRSOL_LOG_DEBUG("Received data from client {}: {}", clientSocket, buffer);
  CommandHandler::processCommand(buffer, clientSocket);
}
} // namespace server
} // namespace irsol
