#include <atomic>
#include <chrono>
#include <csignal>
#include <opencv2/opencv.hpp>
#include <optional>

#include "irsol/irsol.hpp"
#include "sockpp/tcp_connector.h"
#include "sockpp/tcp_socket.h"

// Global flag to indicate shutdown request
std::atomic<bool> g_terminate{false};

// Signal handler function
void signalHandler(int signal) {
  if (signal == SIGTERM || signal == SIGINT) {
    IRSOL_LOG_INFO("Signal {} received, shutting down gracefully...", signal);
    g_terminate = true;
  }
}

cv::Mat createMat(unsigned char *data, int rows, int cols, int chs = 1) {
  // Create Mat from buffer
  cv::Mat mat(rows, cols, CV_MAKETYPE(cv::DataType<unsigned char>::type, chs));
  memcpy(mat.data, data, rows * cols * chs * sizeof(unsigned char));
  return mat;
}

std::optional<cv::Mat> receiveImage(sockpp::tcp_connector &conn) {
  // Step 1: Read ASCII header until ':' is found
  std::string headerTitle;
  char ch;
  while (true) {
    auto res = conn.read(&ch, 1);
    if (res.value() <= 0) {
      IRSOL_LOG_WARN("Error reading header");
      return std::nullopt;
    }
    if (ch == ':')
      break;
    headerTitle += ch;
  }
  if (headerTitle != "image_data") {
    IRSOL_LOG_ERROR("Invalid header title: {}", headerTitle);
    return std::nullopt;
  }

  std::string header;
  while (true) {
    auto res = conn.read(&ch, 1);
    if (res.value() <= 0) {
      IRSOL_LOG_WARN("Error reading header");
      return std::nullopt;
    }
    if (ch == ':')
      break;
    header += ch;
  }
  IRSOL_LOG_DEBUG("Received header: {}", header);

  // Step 2: Parse header like "1920x1080x3"
  int width = 0, height = 0, channels = 0;
  if (sscanf(header.c_str(), "%dx%dx%d", &width, &height, &channels) != 3) {
    IRSOL_LOG_ERROR("Invalid header format: {}", header);
    return std::nullopt;
  }

  int type = (channels == 1) ? CV_8UC1 : (channels == 3) ? CV_8UC3 : (channels == 4) ? CV_8UC4 : -1;
  if (type == -1) {
    IRSOL_LOG_ERROR("Unsupported number of channels: {}", channels);
    return std::nullopt;
  }

  size_t expectedSize = width * height * channels;
  std::vector<uint8_t> buffer(expectedSize);

  // Step 3: Read image data
  size_t totalRead = 0;
  while (totalRead < expectedSize) {
    auto res = conn.read(buffer.data() + totalRead, expectedSize - totalRead);
    if (res.value() <= 0) {
      IRSOL_LOG_ERROR("Error reading image data");
      return std::nullopt;
    }
    totalRead += res.value();
  }

  // Step 4: Convert image data to OpenCV Mat
  return createMat(buffer.data(), height, width, channels);
}

sockpp::tcp_connector
createConnectionWithRetry(const std::string &host, in_port_t port,
                          std::chrono::seconds retryTimeout = std::chrono::seconds(1)) {
  std::error_code ec;
  while (true) {
    sockpp::tcp_connector conn({host, port}, ec);
    if (ec) {
      IRSOL_LOG_WARN("Failed to connect to server at {}:{}: {}, retrying in {} seconds", host, port,
                     ec.message(), retryTimeout.count());
      std::this_thread::sleep_for(retryTimeout);
      continue;
    } else {
      return std::move(conn);
    }
  }
}

void run() {
  irsol::initLogging("log/viewer-client.log");
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("TCP client viewer");

  std::string server_host = "localhost";
  in_port_t port = 15099; // port used by existing clients

  sockpp::initialize();

  sockpp::tcp_connector conn = createConnectionWithRetry(server_host, port);

  IRSOL_LOG_INFO("Connected to server");

  // Set a timeout for the responses
  if (auto res = conn.read_timeout(std::chrono::seconds(2)); !res) {
    IRSOL_LOG_ERROR("Error setting TCP read timeout: {}", res.error_message());
  } else {
    IRSOL_LOG_DEBUG("Read timeout set to 2 seconds");
  }

  auto start = std::chrono::steady_clock::now();
  bool firstFrame = true;

  while (!g_terminate.load()) {
    std::string message = "image?\n";
    size_t sz = message.length();
    if (conn.write(message) != sz) {
      IRSOL_LOG_ERROR("Error writing to the TCP, trying to re-connect..");
      conn = createConnectionWithRetry(server_host, port, std::chrono::seconds(5));
      continue;
    }

    auto imageOpt = receiveImage(conn);
    if (!imageOpt.has_value()) {
      IRSOL_LOG_ERROR("Failed to receive image, waiting 1 second..");
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      continue;
    }
    cv::Mat image = std::move(imageOpt.value());

    if (firstFrame) {
      IRSOL_LOG_INFO("Image received: {}x{} with {} channels", image.rows, image.cols,
                     image.channels());
      firstFrame = false;
    }

    auto now = std::chrono::steady_clock::now();
    auto ms = std::max<int64_t>(
        1, std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
    double fps = 1000.0 / ms;
    IRSOL_LOG_DEBUG("FPS: {:.2f}", fps);
    start = now;

    cv::putText(image, "FPS: " + std::to_string(fps), {20, 80}, cv::FONT_HERSHEY_COMPLEX, 1.0,
                {0, 255, 0}, 1, cv::LINE_AA);

    cv::imshow("Viewer", image);

    int key = cv::waitKey(1) & 0xFF;
    if (key == 27 || key == 'q') {
      IRSOL_LOG_INFO("Exit requested via keyboard");
      break;
    }
  }

  IRSOL_LOG_INFO("Exiting viewer");
}

int main() {
  // Register signal handler
  std::signal(SIGTERM, signalHandler);
  std::signal(SIGINT, signalHandler); // Also handle Ctrl+C

  run();
}