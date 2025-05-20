#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/client_session.hpp"
#include "irsol/server/collector.hpp"
#include <atomic>
#include <mutex>
#include <sockpp/tcp_acceptor.h>
#include <thread>
#include <unordered_map>

namespace irsol {

namespace internal {

// Forward declaration
class ClientSession;
class FrameCollector;
} // namespace internal

class ServerApp {
public:
  explicit ServerApp(uint32_t port);
  bool start();
  void stop();
  void broadcast(const std::string &message);
  CameraInterface &camera();
  internal::FrameCollector &frameCollector();

private:
  const uint32_t m_port;
  std::atomic<bool> m_running;

  std::thread m_acceptThread;
  sockpp::tcp_acceptor m_acceptor;

  std::mutex m_clientsMutex;
  std::unordered_map<std::string, std::shared_ptr<internal::ClientSession>> m_clients;

  std::unique_ptr<CameraInterface> m_cameraInterface;
  std::unique_ptr<internal::FrameCollector> m_frameCollector;

  void acceptLoop();
  void addClient(const std::string &clientId, std::shared_ptr<internal::ClientSession> session);
  void removeClient(const std::string &clientId);
};
} // namespace irsol