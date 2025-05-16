#include "irsol/irsol.hpp"

#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

class ServerExample {
public:
  ServerExample(int port = 15099, int maxClients = 16)
      : m_port(port), m_maxClients(maxClients), m_server(m_port, m_maxClients) {
    m_running = false;

    irsol::init_logging("log/server.log");
    irsol::init_assert_handler();
    // Register signal handler for graceful shutdown
    std::signal(SIGINT, ServerExample::signal_handler);
    std::signal(SIGTERM, ServerExample::signal_handler);
  }

  void run() {
    IRSOL_ASSERT_ERROR(!m_running, "Server is already running!");
    m_running = true;
    try {
      std::thread server_thread([&]() { m_server.run(); });

      IRSOL_LOG_INFO("Server is running on port {0:d}. Press Ctrl+C to stop.", m_port);

      // Wait until the server is stopped
      while (m_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }

      IRSOL_LOG_INFO("Server stopping.");
      m_server.terminate();
      server_thread.join();
    } catch (const std::exception &e) {
      IRSOL_LOG_FATAL("Exception occurred: {}", e.what());
    }
  }

private:
  static void signal_handler(int signal) {
    if (!m_running)
      return; // Ignore signals while server is not running
    if (signal == SIGINT || signal == SIGTERM) {
      IRSOL_LOG_INFO("Received signal {0:d}, stopping server.", signal);
      m_running = false;
    }
  }

  static std::atomic<bool> m_running;
  const int m_port;
  const int m_maxClients;
  irsol::Server m_server;
};

std::atomic<bool> ServerExample::m_running;

int main() {
  ServerExample example;
  example.run();
}
