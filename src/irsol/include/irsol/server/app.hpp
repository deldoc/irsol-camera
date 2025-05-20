#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/client_session.hpp"
#include <atomic>
#include <mutex>
#include <sockpp/tcp_acceptor.h>
#include <thread>
#include <unordered_map>

namespace irsol {

namespace internal {

// Forward declaration
class ClientSession;
} // namespace internal

class ServerApp {
public:
  explicit ServerApp(int port);
  bool start();
  void stop();
  void broadcast(const std::string &message);
  CameraInterface &camera();

private:
  int m_port;
  std::atomic<bool> m_running;
  sockpp::tcp_acceptor m_acceptor;
  std::thread m_acceptThread;
  std::mutex m_clientsMutex;
  std::unordered_map<std::string, std::shared_ptr<internal::ClientSession>> m_clients;
  CameraInterface m_cameraInterface;
  void acceptLoop();
  void removeClient(const std::string &clientId);
};
} // namespace irsol