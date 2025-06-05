#include "irsol/irsol.hpp"

#include <string>
#include <thread>

int
main()
{

  irsol::initLogging("log/camera-server.log");
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("Sample TCP camera server");

  in_port_t port = 15099;  // port used by existing clients

  irsol::server::App app(port);
  if(!app.start()) {
    IRSOL_LOG_FATAL("Failed to start server.");
    return 1;
  }

  IRSOL_LOG_INFO("Server running. Press 'q' to quit.");
  while(true) {
    char c = std::cin.get();
    if(c == 'q') {
      IRSOL_LOG_INFO("Stopping server...");
      app.stop();
      break;
    } else {
      IRSOL_LOG_WARN("Invalid input '{}'. Press 'q' to quit.", c);
    }
  }

  return 0;
}