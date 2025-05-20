#include <atomic>
#include <chrono>
#include <csignal>
#include <opencv2/opencv.hpp>
#include <optional>

#include "args/args.hpp"
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

std::optional<std::pair<size_t, cv::Mat>> receiveImage(sockpp::tcp_connector &conn) {
  // Step 1: Read ASCII header until ':' is found
  std::string headerTitle;
  char ch;

  // Loop until we get the correct header start
  while (true) {
    headerTitle.clear();
    while (true) {
      auto res = conn.read(&ch, 1);
      if (res.value() <= 0)
        return std::nullopt;
      if (ch == ':' || ch == '\n')
        break;
      headerTitle += ch;
    }

    if (headerTitle == "image_data") {
      break; // valid header found
    } else {
      IRSOL_LOG_WARN("Skipping non-image message: {}", headerTitle);
      // skip until next newline or next header
      while (ch != '\n' && conn.read(&ch, 1).value() > 0) {
        continue;
      }
    }
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

  // Step 2: Parse header like "imageIDx1920x1080x3"
  int width = 0, height = 0, channels = 0;
  size_t imageId = 0;
  if (sscanf(header.c_str(), "%lux%dx%dx%d", &imageId, &width, &height, &channels) != 4) {
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
  return std::make_pair(imageId, createMat(buffer.data(), height, width, channels));
}

std::optional<sockpp::tcp_connector>
createConnectionWithRetry(const std::string &host, in_port_t port,
                          std::chrono::seconds retryTimeout = std::chrono::seconds(1)) {
  std::error_code ec;
  while (!g_terminate.load()) {
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
  return std::nullopt;
}

void run(double inFps) {

  IRSOL_LOG_INFO("TCP client viewer");

  std::string server_host = "localhost";
  in_port_t port = 15099; // port used by existing clients

  sockpp::initialize();

  sockpp::tcp_connector conn;
  if (auto connOpt = createConnectionWithRetry(server_host, port); !connOpt.has_value()) {
    return;
  } else {
    conn = std::move(connOpt.value());
  }

  IRSOL_LOG_INFO("Connected to server");

  // Set a timeout for the responses
  if (auto res = conn.read_timeout(std::chrono::seconds(10)); !res) {
    IRSOL_LOG_ERROR("Error setting TCP read timeout: {}", res.error_message());
  } else {
    IRSOL_LOG_DEBUG("Read timeout set to 10 seconds");
  }

  auto start = std::chrono::steady_clock::now();
  bool firstFrame = true;

  // Start listening for incoming images
  std::string query = "start_frame_listening=" + std::to_string(inFps) + "\n";
  IRSOL_LOG_INFO("Starting frame listening with FPS: {}", inFps);
  conn.write(query);

  bool fpsUpdated = false;

  while (!g_terminate.load()) {

    auto imageOpt = receiveImage(conn);
    if (!imageOpt.has_value()) {
      IRSOL_LOG_ERROR("Failed to receive image, waiting 1 second..");
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      continue;
    }
    auto [imageId, image] = std::move(imageOpt.value());
    if (image.empty()) {
      IRSOL_LOG_INFO("Received empty image, waiting 1 second..");
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      continue;
    }

    if (firstFrame) {
      IRSOL_LOG_INFO("Image {} received: {}x{} with {} channels", imageId, image.rows, image.cols,
                     image.channels());
      firstFrame = false;
    }

    auto now = std::chrono::steady_clock::now();
    auto ms = std::max<int64_t>(
        1, std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
    double measuredFps = 1000.0 / ms;
    IRSOL_LOG_DEBUG("Measured FPS: {:.2f}", measuredFps);
    start = now;
    cv::putText(image, "ImageID: " + std::to_string(imageId), {20, 80}, cv::FONT_HERSHEY_COMPLEX,
                1.0, {0, 255, 0}, 1, cv::LINE_AA);

    cv::putText(image, "Measured FPS: " + std::to_string(measuredFps), {20, 160},
                cv::FONT_HERSHEY_COMPLEX, 1.0, {0, 255, 0}, 1, cv::LINE_AA);

    cv::imshow("Viewer", image);

    int key = cv::waitKey(1) & 0xFF;
    if (key == 27 || key == 'q') {
      IRSOL_LOG_INFO("Exit requested via keyboard");
      break;
    }

    if (imageId > 250 && !fpsUpdated) {
      IRSOL_LOG_INFO("Updating FPS to double: {:.2f}", inFps * 2);
      std::string query = "start_frame_listening=" + std::to_string(inFps * 2) + "\n";
      IRSOL_LOG_INFO("Starting frame listening with FPS: {}", inFps);
      conn.write(query);
      fpsUpdated = true;
    }
  }

  conn.write("stop_frame_listening\n");

  IRSOL_LOG_INFO("Exiting viewer");
}

int main(int argc, char **argv) {
  irsol::initLogging("log/viewer-client.log");
  irsol::initAssertHandler();

  // Register signal handler
  std::signal(SIGTERM, signalHandler);
  std::signal(SIGINT, signalHandler); // Also handle Ctrl+C

  args::ArgumentParser parser("TCP client viewer");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<double> listen_fps_flag(parser, "fps", "The FPS at which to listen.", {'f'});
  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help) {
    IRSOL_LOG_INFO("{0:s}", parser.Help());
    return 0;
  } catch (args::ParseError e) {
    IRSOL_LOG_ERROR("Error parsing command-line arguments: {}\n", e.what(), parser.Help());
    return 1;
  } catch (args::ValidationError e) {
    IRSOL_LOG_ERROR("Error parsing command-line arguments: {}\n", e.what(), parser.Help());
    return 1;
  }
  double listen_fps = 5.0;
  if (listen_fps_flag) {
    listen_fps = args::get(listen_fps_flag);
  }

  run(listen_fps);
}