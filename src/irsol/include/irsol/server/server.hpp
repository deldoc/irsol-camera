#pragma once

#include <poll.h>
#include <vector>

namespace irsol {

class Server {
public:
    Server(int port, int maxClients);
    ~Server();

    void run();
    void terminate();

private:
    int m_port;
    int m_maxClients;
    int m_serverSocket;
    bool m_terminate;
    std::vector<struct pollfd> m_fds;

    void initServer();
    void acceptConnection();
    void handleClient(int clientSocket);
};

} // namespace irsol
