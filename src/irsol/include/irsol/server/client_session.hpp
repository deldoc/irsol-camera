#pragma once

#include "irsol/server/app.hpp"
#include "sockpp/tcp_socket.h"
#include <memory>
#include <mutex>
#include <string>

namespace irsol {

// Forward declaration
class ServerApp;

namespace internal {

struct SessionData {

  SessionData(sockpp::tcp_socket &&sock) noexcept
      : m_sock(std::move(sock))
  // m_mutex is default-initialized
  {}

  sockpp::tcp_socket m_sock;
  std::mutex m_mutex{};

  std::string address() const { return m_sock.address().to_string(); }
};

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
  ClientSession(const std::string &id, sockpp::tcp_socket &&sock, ServerApp &app);
  void run();
  void send(const std::string &message);
  void send(void *data, size_t size);

  const ServerApp &app() const { return m_app; }
  ServerApp &app() { return m_app; }
  const std::string &id() const { return m_id; }
  const SessionData &sessionData() const { return m_sessionData; }
  SessionData &sessionData() { return m_sessionData; }

private:
  void processMessageBuffer(std::string &messageBuffer);
  void processRawMessage(const std::string &rawMessage);
  std::string m_id;
  SessionData m_sessionData;
  ServerApp &m_app;
};
} // namespace internal
} // namespace irsol