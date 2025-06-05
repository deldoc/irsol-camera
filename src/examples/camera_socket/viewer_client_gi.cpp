#include "irsol/args.hpp"
#include "irsol/irsol.hpp"

#include <atomic>
#include <csignal>
#include <opencv2/opencv.hpp>
#include <optional>

// Global flag to indicate shutdown request
std::atomic<bool> g_terminate{false};

// Signal handler function
void
signalHandler(int signal)
{
  if(signal == SIGTERM || signal == SIGINT) {
    IRSOL_LOG_INFO("Signal {} received, shutting down gracefully...", signal);
    g_terminate = true;
  }
}

cv::Mat
createMat(unsigned char* data, int rows, int cols)
{
  // Create Mat from buffer
  cv::Mat mat(rows, cols, CV_16UC1);
  memcpy(mat.data, data, rows * cols * 2);
  // Scale the values from the 12-bit depth to 16-bit depth
  mat *= irsol::camera::PixelConversion<12, 16>::factor;
  return mat;
}

std::optional<std::pair<size_t, cv::Mat>>
queryImage(irsol::types::connector_t& conn)
{
  conn.write("gi\n");

  // Loop until we get the correct header start
  while(true) {
    std::string headerTitle;
    char        ch;
    while(true) {
      auto res = conn.read(&ch, 1);
      if(res.value() <= 0)
        return std::nullopt;
      if(ch == '\n' || ch == '=')
        break;
      headerTitle += ch;
    }

    if(headerTitle == "gi;") {
      IRSOL_LOG_INFO("Received success confirmation from server");
    } else if(headerTitle == "isn") {
      std::string isnStr;
      while(ch != '\n') {
        conn.read(&ch, 1);
        isnStr.insert(isnStr.end(), ch);
      }
      uint64_t isn = std::stoull(isnStr);
      IRSOL_LOG_INFO("Received input sequence number {}", isn);
    } else if(headerTitle == "img") {
      IRSOL_LOG_INFO("Received 'img' header");

      uint32_t imageHeight{0};
      uint32_t imageWidth{0};
      while(true) {
        auto res = conn.read(&ch, 1);
        if(res.value() <= 0) {
          IRSOL_LOG_ERROR("Failed to read image metadata");
          return std::nullopt;
        }
        if(ch == irsol::utils::bytesToString({irsol::protocol::Serializer::SpecialBytes::SOH})[0]) {
          // Parse the image shape
          while(ch != '[') {
            conn.read(&ch, 1);
          }
          std::string heightStr;
          while(ch != ',') {
            conn.read(&ch, 1);
            heightStr.insert(heightStr.end(), ch);
          }
          imageHeight = std::stol(heightStr);
          std::string widthStr;
          while(ch != ']') {
            conn.read(&ch, 1);
            widthStr.insert(widthStr.end(), ch);
          }
          imageWidth = std::stol(widthStr);
          // Here we start the image attributes, skip them
          while(ch !=
                irsol::utils::bytesToString({irsol::protocol::Serializer::SpecialBytes::STX})[0]) {
            conn.read(&ch, 1);
          }
          break;
        }
      }

      // Now we can read the image bytes
      uint64_t             expectedSize = imageHeight * imageWidth * 2;  // 2 bytes per pixel
      std::vector<uint8_t> buffer(expectedSize);

      // Step 3: Read image data
      size_t totalRead = 0;
      while(totalRead < expectedSize) {
        auto res = conn.read(buffer.data() + totalRead, expectedSize - totalRead);
        if(res.value() <= 0) {
          IRSOL_LOG_ERROR("Error reading image data");
          return std::nullopt;
        }
        totalRead += res.value();
      }
      // Step 5: Convert image data to OpenCV Mat
      return std::make_pair(1, createMat(buffer.data(), imageHeight, imageWidth));
    } else {

      IRSOL_LOG_WARN("Skipping unknown header: '{}'", headerTitle);
      // Skip the rest of the line
      while(ch != '\n') {
        auto res = conn.read(&ch, 1);
        if(res.value() <= 0)
          return std::nullopt;
      }
    }
  }
}

std::optional<irsol::types::connector_t>
createConnectionWithRetry(
  const std::string&   host,
  irsol::types::port_t port,
  std::chrono::seconds retryTimeout = std::chrono::seconds(1))
{
  std::error_code ec;
  while(!g_terminate.load()) {
    irsol::types::connector_t conn({host, port}, ec);
    if(ec) {
      IRSOL_LOG_WARN(
        "Failed to connect to server at {}:{}: {}, retrying in {} seconds",
        host,
        port,
        ec.message(),
        retryTimeout.count());
      std::this_thread::sleep_for(retryTimeout);
      continue;
    } else {
      return std::move(conn);
    }
  }
  return std::nullopt;
}

void
run(double inFps)
{

  IRSOL_LOG_INFO("TCP client viewer (gi)");

  std::string          server_host = "localhost";
  irsol::types::port_t port        = 15099;  // port used by existing clients

  sockpp::initialize();

  irsol::types::connector_t conn;
  if(auto connOpt = createConnectionWithRetry(server_host, port); !connOpt.has_value()) {
    return;
  } else {
    conn = std::move(connOpt.value());
  }

  IRSOL_LOG_INFO("Connected to server");

  // Set a timeout for the responses
  if(auto res = conn.read_timeout(std::chrono::seconds(10)); !res) {
    IRSOL_LOG_ERROR("Error setting TCP read timeout: {}", res.error_message());
  } else {
    IRSOL_LOG_DEBUG("Read timeout set to 10 seconds");
  }

  auto lastTimeShown = irsol::types::clock_t::now() - std::chrono::seconds(1000);

  bool firstFrame = true;

  // Start listening for incoming images

  IRSOL_LOG_INFO("Starting frame polling with FPS: {}", inFps);

  while(!g_terminate.load()) {
    auto frameStart = irsol::types::clock_t::now();

    // --- Query image ---
    auto imageOpt = queryImage(conn);

    if(!imageOpt.has_value()) {
      IRSOL_LOG_ERROR("Failed to receive image, waiting 1 second..");
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    auto [imageId, image] = std::move(imageOpt.value());
    if(image.empty()) {
      IRSOL_LOG_INFO("Received empty image, waiting 1 second..");
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    // --- Annotate ---
    if(firstFrame) {
      IRSOL_LOG_INFO(
        "Image {} received: {}x{} with {} channels",
        imageId,
        image.rows,
        image.cols,
        image.channels());
      firstFrame = false;
    }

    cv::putText(
      image,
      "ImageID: " + std::to_string(imageId),
      {20, 80},
      cv::FONT_HERSHEY_COMPLEX,
      1.0,
      {65535, 65535, 65535},
      1,
      cv::LINE_AA);

    // Show current measured FPS
    auto   frameNow  = irsol::types::clock_t::now();
    double actualFps = 1.0 / std::chrono::duration<double>(frameNow - lastTimeShown).count();
    lastTimeShown    = frameNow;

    IRSOL_LOG_DEBUG("Measured FPS: {:.2f}", actualFps);

    cv::putText(
      image,
      "Measured FPS: " + std::to_string(actualFps),
      {20, 160},
      cv::FONT_HERSHEY_COMPLEX,
      1.0,
      {65535, 65535, 65535},
      1,
      cv::LINE_AA);

    // --- Display ---
    cv::imshow("Viewer", image);
    int key = cv::waitKey(1) & 0xFF;
    if(key == 27 || key == 'q') {
      IRSOL_LOG_INFO("Exit requested via keyboard");
      g_terminate.store(true);
    }

    // --- Frame duration regulation ---
    auto frameEnd      = irsol::types::clock_t::now();
    auto frameDuration = frameEnd - frameStart;

    auto desiredFrameTime = std::chrono::microseconds(static_cast<int64_t>(1'000'000.0 / inFps));
    if(frameDuration < desiredFrameTime) {
      std::this_thread::sleep_for(desiredFrameTime - frameDuration);
    }
  }

  IRSOL_LOG_INFO("Exiting viewer");
}

int
main(int argc, char** argv)
{
  irsol::initLogging("logs/viewer-client-poll.log");
  irsol::initAssertHandler();

  // Register signal handler
  std::signal(SIGTERM, signalHandler);
  std::signal(SIGINT, signalHandler);  // Also handle Ctrl+C

  args::ArgumentParser    parser("TCP client viewer");
  args::HelpFlag          help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<double> listen_fps_flag(parser, "fps", "The FPS at which to listen.", {'f'});
  try {
    parser.ParseCLI(argc, argv);
  } catch(args::Help) {
    IRSOL_LOG_INFO("{0:s}", parser.Help());
    return 0;
  } catch(args::ParseError e) {
    IRSOL_LOG_ERROR("Error parsing command-line arguments: {}\n", e.what(), parser.Help());
    return 1;
  } catch(args::ValidationError e) {
    IRSOL_LOG_ERROR("Error parsing command-line arguments: {}\n", e.what(), parser.Help());
    return 1;
  }
  double listen_fps = 0.5;
  if(listen_fps_flag) {
    listen_fps = args::get(listen_fps_flag);
  }

  run(listen_fps);
}