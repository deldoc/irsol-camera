#include <thread>

#include "irsol/irsol.hpp"
#include "sockpp/tcp_acceptor.h"

void handle_socket_connection(sockpp::tcp_socket sock) {
  char buf[512];
  sockpp::result<size_t> res;

  IRSOL_LOG_INFO("Connection established");

  while ((res = sock.read(buf, sizeof(buf))) && res.value() > 0) {
    IRSOL_LOG_DEBUG("Received {}: {}", res.value(), std::string(buf, res.value()));
    sock.write_n(buf, res.value());
  }

  IRSOL_LOG_INFO("Connection closed");
}

int main() {

  irsol::initLogging("log/socket-server.log");
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("Sample TCP echo server for 'sockpp'");

  in_port_t port = sockpp::TEST_PORT;

  sockpp::initialize();

  std::error_code ec;
  sockpp::tcp_acceptor acc{port, 4, ec};

  if (ec) {
    IRSOL_LOG_ERROR("Error creating the acceptor: {}", ec.message());
    return 1;
  }

  IRSOL_LOG_INFO("Starting acceptor loop. Awaiting connections on port {}", port);
  while (true) {
    sockpp::inet_address peer;

    // Accept a new client connection
    if (auto res = acc.accept(&peer); !res) {
      IRSOL_LOG_ERROR("Error accepting incoming connection: {}", ec.message());
    } else {
      IRSOL_LOG_INFO("Accepted connection from");
      sockpp::tcp_socket sock = res.release();

      // Create a thread and transfer the new stream to it.
      std::thread thr(handle_socket_connection, std::move(sock));
      thr.detach();
      IRSOL_LOG_DEBUG("Started new thread for client {}", peer.to_string());
    }
  }

  return 0;
}