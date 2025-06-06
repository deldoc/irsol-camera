/**
 * @file examples/06-client-server-interaction-image-commands/server.cpp
 * @brief Example camera server for client-server interaction with image commands.
 *
 * This executable launches a TCP camera server using the IRSOL framework.
 * It listens for incoming client connections and processes image acquisition commands,
 * such as `gi` (get image) and `gis` (get image stream), distributing frames to clients as
 * requested.
 *
 * Usage:
 *   ./06-client-server-interaction-image-commands-server
 *
 * The server runs until the user presses 'q' in the terminal, at which point it shuts down
 * gracefully. All logging is written to logs/camera-server.log.
 */

#include "irsol/irsol.hpp"

#include <string>
#include <thread>

int
main()
{

  irsol::initLogging("logs/camera-server.log");
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