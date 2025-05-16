#include "args/args.hpp"
#include "irsol/irsol.hpp"

#include <chrono>
#include <csignal>
#include <iostream>
#include <stdint.h>
#include <thread>

class ServerExample {
public:
  ServerExample(uint16_t port = 15099, uint16_t maxClients = 16)
      : m_port(port), m_maxClients(maxClients), m_server(m_port, m_maxClients) {
    m_running = false;

    irsol::initLogging("log/server.log");
    irsol::initAssertHandler();
    // Register signal handler for graceful shutdown
    std::signal(SIGINT, ServerExample::signal_handler);
    std::signal(SIGTERM, ServerExample::signal_handler);
  }

  static ServerExample fromArgs(int argc, char *argv[]) {
    const uint16_t defaultPort = 15099;
    const uint16_t defaultMaxClients = 16;
    args::ArgumentParser parser("This is a test program running the camera server");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<uint16_t> port(parser, "port",
                                   "Port to listen on, defaults to " + std::to_string(defaultPort),
                                   {'p', "port"});
    args::ValueFlag<uint16_t> maxClients(
        parser, "maxClients",
        "Number of clients allowed to connect simultaneously, defaults to " +
            std::to_string(defaultMaxClients),
        {'c', "num-clients"});

    try {
      parser.ParseCLI(argc, argv);
    } catch (args::Help) {
      IRSOL_LOG_INFO("{0:s}", parser.Help());
      std::exit(1);
    } catch (args::ParseError e) {
      IRSOL_LOG_ERROR("Error parsing command line arguments: {}", e.what());
      IRSOL_LOG_INFO("{0:s}", parser.Help());
      std::exit(1);
    } catch (args::ValidationError e) {
      IRSOL_LOG_ERROR("Error parsing command line arguments: {}", e.what());
      IRSOL_LOG_INFO("{0:s}", parser.Help());
      std::exit(1);
    }

    uint16_t portValue;
    if (port) {
      portValue = args::get(port);
    } else {
      IRSOL_LOG_DEBUG("No port specified, using default port 15099.");
      portValue = defaultPort;
    }

    uint16_t maxClientsValue;
    if (maxClients) {
      maxClientsValue = args::get(maxClients);
    } else {
      IRSOL_LOG_DEBUG("No maximum clients specified, using default 16.");
      maxClientsValue = defaultMaxClients;
    }

    return ServerExample(portValue, maxClientsValue);
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
  const uint16_t m_port;
  const uint16_t m_maxClients;
  irsol::Server m_server;
};

std::atomic<bool> ServerExample::m_running;

int main(int argc, char *argv[]) {
  ServerExample example = ServerExample::fromArgs(argc, argv);
  example.run();
}
