#include "irsol/server/server.hpp"
#include "irsol/server/command_handler.hpp"
#include "irsol/logging.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace irsol {

Server::Server(int port, int maxClients)
    : m_port(port), m_maxClients(maxClients), m_terminate(false), m_fds(maxClients) {
    IRSOL_LOG_INFO("Initializing server on port {}", m_port);
    for (auto& fd : m_fds) {
        fd.fd = 0;
        fd.events = 0;
    }
    m_fds[0].fd = -1; // Placeholder for server socket initialization
    m_fds[0].events = POLLIN;
    initServer();
}

Server::~Server() {
    close(m_serverSocket);
    IRSOL_LOG_INFO("Server shut down.");
}

void Server::initServer() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        IRSOL_LOG_FATAL("Failed to create socket: {}", strerror(errno));
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        IRSOL_LOG_FATAL("Failed to bind socket: {}", strerror(errno));
        throw std::runtime_error("Socket bind failed");
    }

    if (listen(m_serverSocket, m_maxClients) < 0) {
        IRSOL_LOG_FATAL("Failed to listen on socket: {}", strerror(errno));
        throw std::runtime_error("Socket listen failed");
    }

    m_fds[0].fd = m_serverSocket; // Assign server socket to fds[0]
    IRSOL_LOG_INFO("Server initialized successfully on port {}", m_port);
}

void Server::run() {
    IRSOL_LOG_INFO("Server is running...");
    while (!m_terminate) {
        int pollCount = poll(m_fds.data(), m_maxClients, 1000);
        if (pollCount < 0) {
            IRSOL_LOG_ERROR("Poll error: {}", strerror(errno));
            continue;
        }

        for (int i = 0; i < m_maxClients; ++i) {
            if(m_terminate) break;
            if (m_fds[i].revents & POLLIN) {
                if (m_fds[i].fd == m_serverSocket) {
                    acceptConnection();
                } else {
                    handleClient(m_fds[i].fd);
                }
            }
        }
    }
}

void Server::terminate() {
    IRSOL_LOG_INFO("Server is stopping...");
    m_terminate = true;
}

void Server::acceptConnection() {
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(m_serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientSocket < 0) {
        IRSOL_LOG_WARN("Failed to accept connection: {}", strerror(errno));
        return;
    }

    IRSOL_LOG_INFO("Accepted new connection: socket {}", clientSocket);
    for (auto& fd : m_fds) {
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

} // namespace irsol
