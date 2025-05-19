#pragma once

#include "irsol/server/app.hpp"
#include <memory>
#include <mutex>
#include <sockpp/tcp_socket.h>

namespace irsol {

// Forward declaration
class ServerApp;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
  ClientSession(sockpp::tcp_socket &&sock, ServerApp &app);
  void run();
  void send(const std::string &message);

  const ServerApp &app() const { return m_app; }
  ServerApp &app() { return m_app; }

private:
  void processMessageBuffer(std::string &messageBuffer);
  void processRawMessage(const std::string &rawMessage);
  sockpp::tcp_socket m_sock;
  ServerApp &m_app;
  std::mutex m_sendMutex;
};
} // namespace irsol