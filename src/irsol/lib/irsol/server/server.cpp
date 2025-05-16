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

Server::Server(int port, int max_clients)
    : m_port(port), m_maxClients(max_clients), m_terminate(false), m_fds(max_clients) {
    IRSOL_LOG_INFO("Initializing server on port {}", m_port);
    for (auto& fd : m_fds) {
        fd.fd = 0;
        fd.events = 0;
    }
    m_fds[0].fd = -1; // Placeholder for server socket initialization
    m_fds[0].events = POLLIN;
    init_server();
}

Server::~Server() {
    close(m_serverSocket);
    IRSOL_LOG_INFO("Server shut down.");
}

void Server::init_server() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        IRSOL_LOG_FATAL("Failed to create socket: {}", strerror(errno));
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
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
        int poll_count = poll(m_fds.data(), m_maxClients, 1000);
        if (poll_count < 0) {
            IRSOL_LOG_ERROR("Poll error: {}", strerror(errno));
            continue;
        }

        for (int i = 0; i < m_maxClients; ++i) {
            if(m_terminate) break;
            if (m_fds[i].revents & POLLIN) {
                if (m_fds[i].fd == m_serverSocket) {
                    accept_connection();
                } else {
                    handle_client(m_fds[i].fd);
                }
            }
        }
    }
}

void Server::terminate() {
    IRSOL_LOG_INFO("Server is stopping...");
    m_terminate = true;
}

void Server::accept_connection() {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(m_serverSocket, (struct sockaddr*)&client_addr, &client_len);

    if (client_socket < 0) {
        IRSOL_LOG_WARN("Failed to accept connection: {}", strerror(errno));
        return;
    }

    IRSOL_LOG_INFO("Accepted new connection: socket {}", client_socket);
    for (auto& fd : m_fds) {
        if (fd.fd == 0) {
            fd.fd = client_socket;
            fd.events = POLLIN;
            break;
        }
    }
}

void Server::handle_client(int client_socket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        IRSOL_LOG_INFO("Client disconnected: socket {}", client_socket);
        close(client_socket);
        return;
    }

    IRSOL_LOG_DEBUG("Received data from client {}: {}", client_socket, buffer);
    CommandHandler::process_command(buffer, client_socket);
}

} // namespace irsol
