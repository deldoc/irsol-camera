#pragma once

#include "irsol/server/app.hpp"
#include <memory>
#include <mutex>
#include <sockpp/tcp_socket.h>
#include <string>

namespace irsol {

// Forward declaration
class ServerApp;

namespace internal {
class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
  ClientSession(const std::string &id, sockpp::tcp_socket &&sock, ServerApp &app);
  void run();
  void send(const std::string &message);

  const ServerApp &app() const { return m_app; }
  ServerApp &app() { return m_app; }
  const std::string &id() const { return m_id; }

private:
  void processMessageBuffer(std::string &messageBuffer);
  void processRawMessage(const std::string &rawMessage);
  std::string m_id;
  sockpp::tcp_socket m_sock;
  ServerApp &m_app;
  std::mutex m_sendMutex;
};
} // namespace internal
} // namespace irsol