#pragma once

#include "irsol/server/camera_controller.hpp"
#include "irsol/server/client_session.hpp"
#include <atomic>
#include <mutex>
#include <sockpp/tcp_acceptor.h>
#include <thread>
#include <unordered_set>

namespace irsol {
// Forward declaration
class ClientSession;

class ServerApp {
public:
  explicit ServerApp(int port);
  bool start();
  void stop();
  void broadcast(const std::string &message);
  CameraController &camera();

private:
  int m_port;
  std::atomic<bool> m_running;
  sockpp::tcp_acceptor m_acceptor;
  std::thread m_acceptThread;
  std::mutex m_clientsMutex;
  std::unordered_set<std::shared_ptr<ClientSession>> m_clients;
  CameraController m_cameraController;

  void acceptLoop();
  void removeClient(const std::shared_ptr<ClientSession> &client);
};
} // namespace irsol