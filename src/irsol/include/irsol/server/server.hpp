#pragma once

#include <poll.h>
#include <vector>

namespace irsol {

class Server {
public:
    Server(int port, int max_clients);
    ~Server();

    void run();
    void terminate();

private:
    int m_port;
    int m_maxClients;
    int m_serverSocket;
    bool m_terminate;
    std::vector<struct pollfd> m_fds;

    void init_server();
    void accept_connection();
    void handle_client(int client_socket);
};

} // namespace irsol
