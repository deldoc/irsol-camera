#pragma once

#include <poll.h>
#include <vector>
#include <stdint.h>

namespace irsol {

class Server {
public:
    Server(uint16_t port, uint16_t maxClients);
    ~Server();

    void run();
    void terminate();

private:
    uint16_t m_port;
    uint16_t m_maxClients;
    int m_serverSocket;
    bool m_terminate;
    std::vector<struct pollfd> m_fds;

    void initServer();
    void acceptConnection();
    void handleClient(int clientSocket);
};

} // namespace irsol
