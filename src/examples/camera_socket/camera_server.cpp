#include <string>
#include <thread>

#include "irsol/irsol.hpp"
#include "sockpp/tcp_acceptor.h"

int main() {

  irsol::initLogging("log/camera-server.log");
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("Sample TCP echo server for 'camera server'");

  in_port_t port = 15099; // port used by existing clients

  irsol::ServerApp app(port); // TCP port
  if (!app.start()) {
    IRSOL_LOG_FATAL("Failed to start server.");
    return 1;
  }

  IRSOL_LOG_INFO("Server running. Press ENTER to quit.");
  std::cin.get();
  app.stop();

  return 0;
}