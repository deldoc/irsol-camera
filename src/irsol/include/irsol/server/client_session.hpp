#pragma once

#include "sockpp/tcp_socket.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>

namespace irsol {

// Forward declaration
class ServerApp;

namespace internal {

struct FrameListeningParams {
  std::atomic<bool> active{false};
  std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameSent{
      std::chrono::high_resolution_clock::now()};
  double frameRate;
};

struct UserSessionData {

  UserSessionData(sockpp::tcp_socket &&sock);

  FrameListeningParams frameListeningParams{};

  sockpp::tcp_socket sock;
  std::mutex mutex{};
};

class ClientSession {
public:
  ClientSession(const std::string &id, sockpp::tcp_socket &&sock, ServerApp &app);
  void run();

  void send(const std::string &message);
  void send(void *data, size_t size);

  const ServerApp &app() const { return m_app; }
  ServerApp &app() { return m_app; }
  const std::string &id() const { return m_id; }
  const UserSessionData &sessionData() const { return m_sessionData; }
  UserSessionData &sessionData() { return m_sessionData; }

private:
  void processMessageBuffer(std::string &messageBuffer);
  void processRawMessage(const std::string &rawMessage);
  std::string m_id;
  UserSessionData m_sessionData;
  ServerApp &m_app;
};
} // namespace internal
} // namespace irsol