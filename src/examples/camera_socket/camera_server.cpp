#include <string>
#include <thread>

#include "irsol/irsol.hpp"
#include "sockpp/tcp_acceptor.h"

void handle_query(const std::string &query, sockpp::tcp_socket &sock, NeoAPI::Cam &cam) {
  if (query == "bypass gi") {
    IRSOL_LOG_INFO("Retrieving Image from camera");
    auto image = cam.GetImage();
    auto data = image.GetImageData();
    // Convert the image data to the socket as bytes
    IRSOL_LOG_DEBUG("Image timestamp: {0:d}", image.GetTimestamp());
    sock.write_n(data, image.GetSize());
    return;
  }
  if (query == "bypass fps?") {
    IRSOL_LOG_INFO("Retrieving frame rate from camera");
    double fps = cam.f().AcquisitionFrameRate;
    char fps_str[16];
    sprintf(fps_str, "fps=%.1f\n", fps);
    IRSOL_LOG_DEBUG("Sending frame rate to client: '{0:s}'.", fps_str);
    sock.write_n(fps_str, strlen(fps_str));
    return;
  }
  IRSOL_LOG_INFO("Unknown query: '{0:s}'", query);
}

void handle_command(const std::string &command, const std::string &value, sockpp::tcp_socket &sock,
                    NeoAPI::Cam &cam) {
  IRSOL_LOG_INFO("Handling command: '{0:s}' with value '{1:s}'", command, value);

  if (command == "bypass fr") {
    double fps = std::stod(value);
    IRSOL_LOG_INFO("Setting frame rate to {0:f}", fps);
    cam.f().AcquisitionFrameRateEnable = true;
    cam.f().AcquisitionFrameRate = fps;

    char fps_str[256];
    sprintf(fps_str, "cfr=%.1f\nfr=%.1f\nfrs=0\nfrrf=1\nisl=0", fps, fps);
    IRSOL_LOG_DEBUG("Sending new frame rate to client: '{0:s}'.", fps_str);
    sock.write_n(fps_str, strlen(fps_str));

    return;
  }
  IRSOL_LOG_INFO("Unknown command: '{0:s}'", command);
}

void process_message(const std::string &message, sockpp::tcp_socket &sock, NeoAPI::Cam &cam) {
  size_t pos = message.find('=');
  if (pos == std::string::npos) {
    // This is a Query
    handle_query(irsol::utils::strip(message), sock, cam);
  } else {
    // This is a Command
    std::string command = irsol::utils::strip(message.substr(0, pos));
    std::string value = irsol::utils::strip(message.substr(pos + 1));
    handle_command(command, value, sock, cam);
  }
}

void handle_socket_connection(sockpp::tcp_socket sock, NeoAPI::Cam &cam) {
  char buf[512];
  sockpp::result<size_t> in_result;

  IRSOL_LOG_INFO("Connection established");

  while ((in_result = sock.read(buf, sizeof(buf))) && in_result.value() > 0) {
    std::string in_message(buf, in_result.value());
    // Split the input message into a list of messages. Each message is separated by a newline.
    std::vector<std::string> messages = irsol::utils::split(in_message, '\n');

    for (const auto &message : messages) {
      process_message(irsol::utils::strip(message), sock, cam);
    }
  }

  IRSOL_LOG_INFO("Connection closed");
}

int main() {

  irsol::initLogging("log/camera-server.log");
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("Sample TCP echo server for 'camera server'");

  in_port_t port = 15099; // port used by existing clients

  sockpp::initialize();

  std::error_code ec;
  const int queue_size = 4;
  sockpp::tcp_acceptor acc(port, queue_size, ec);

  if (ec) {
    IRSOL_LOG_ERROR("Error creating the acceptor on port {0:d}: {1:s}", port, ec.message());
    return 1;
  }

  NeoAPI::Cam cam = irsol::utils::loadDefaultCamera();
  // Start monitoring thread
  irsol::CameraStatusMonitor monitor(cam, std::chrono::milliseconds(5000));
  monitor.start();

  IRSOL_LOG_INFO("Starting acceptor loop. Awaiting connections on port {0:d}", port);
  while (true) {
    sockpp::inet_address peer;

    // Accept a new client connection
    if (auto res = acc.accept(&peer); !res) {
      IRSOL_LOG_ERROR("Error accepting incoming connection: {0:s}", ec.message());
    } else {
      IRSOL_LOG_INFO("Accepted connection from");

      // Create a thread and transfer the new stream to it.
      std::thread th{handle_socket_connection, res.release(),
                     std::ref(cam)}; // Move the socket object to the thread.
      th.detach();                   // Detach the thread to avoid blocking the main thread.
      IRSOL_LOG_DEBUG("Started new thread for client {0:s}", peer.to_string());
    }
  }

  return 0;
}