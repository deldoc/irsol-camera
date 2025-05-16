#pragma once

#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <sys/socket.h>
#include <vector>

namespace irsol {

namespace server {

namespace internal {

const int SERVER_FD_IDX = 0; // index for fs of server
const int STDIN_FD_IDX = 1;  // index for fs of stdin

const int ADDRESS_FAMILY = AF_INET;
const int SOCKET_TYPE = SOCK_STREAM;
const int PROTOCOL = IPPROTO_TCP;

const int IN_ADDRESS_ANY = INADDR_ANY;

} // namespace internal

class Server {
public:
  Server(uint16_t port, uint16_t maxClients, int16_t stdInputCommunicationId = -1);
  ~Server();

  void run();
  void terminate();

private:
  const uint16_t m_port;
  const uint16_t m_maxClients;
  const int16_t m_stdInputCommunicationId;

  int m_serverSocketId;
  bool m_terminate;
  std::vector<struct pollfd> m_fds;

  void initServer();
  void acceptConnection();
  void handleClient(int clientSocket);
};
} // namespace server
} // namespace irsol
