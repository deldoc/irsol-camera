#include "args/args.hpp"
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
createMat(unsigned char* data, int rows, int cols, int chs = 1)
{
  // Create Mat from buffer
  cv::Mat mat(rows, cols, CV_MAKETYPE(cv::DataType<unsigned char>::type, chs));
  memcpy(mat.data, data, rows * cols * chs * sizeof(unsigned char));
  return mat;
}

bool
configureGis(irsol::types::connector_t& conn, double fps, uint64_t isl)
{
  {
    std::stringstream ss;
    ss << "fr=" << std::to_string(fps) << "\n";
    conn.write(ss.str());

    // Check we received the expected responses
    std::string response;
    char        ch;
    while(true) {
      auto res = conn.read(&ch, 1);
      if(res.value() <= 0) {
        IRSOL_LOG_ERROR("Failed to read header");
        return false;
      }
      if(ch == '\n')
        break;
      response += ch;
    }
    IRSOL_LOG_INFO("Response for FPS: '{}'", response);
  }
  {
    std::stringstream ss;
    ss << "isl=" << std::to_string(isl) << "\n";
    conn.write(ss.str());

    // Check we received the expected responses
    std::string response;
    char        ch;
    while(true) {
      auto res = conn.read(&ch, 1);
      if(res.value() <= 0) {
        IRSOL_LOG_ERROR("Failed to read header");
        return false;
      }
      if(ch == '\n')
        break;
      response += ch;
    }
    std::string expectedIslResponse = "isl=" + std::to_string(isl);
    if(response != expectedIslResponse) {
      IRSOL_LOG_ERROR(
        "Expected response for ISL command is '{}', but got '{}'", expectedIslResponse, response);
      return false;
    }
    IRSOL_LOG_INFO("Response for isl: '{}'", response);
  }
  return true;
}

std::optional<std::vector<std::pair<size_t, cv::Mat>>>
queryImages(irsol::types::connector_t& conn)
{
  auto                                   t0 = irsol::types::clock_t::now();
  std::vector<irsol::types::timepoint_t> retrieveTimes;

  conn.write("gis\n");

  std::vector<std::pair<size_t, cv::Mat>> result;

  while(true) {
    // Read header title until ':' or '\n'
    std::string headerTitle;
    char        ch;
    while(true) {
      auto res = conn.read(&ch, 1);
      if(res.value() <= 0) {
        IRSOL_LOG_ERROR("Failed to read header title");
        return std::nullopt;
      }
      if(ch == ':' || ch == '\n')
        break;
      headerTitle += ch;
    }

    if(headerTitle == "gis;") {
      IRSOL_LOG_INFO("Received completion signal from server");
      break;  // Done
    }

    if(headerTitle != "image_data") {
      IRSOL_LOG_WARN("Skipping unknown header: {}", headerTitle);
      // Skip the rest of the line
      while(ch != '\n') {
        auto res = conn.read(&ch, 1);
        if(res.value() <= 0)
          return std::nullopt;
      }
      continue;
    }

    // Read image metadata after "image_data:"
    std::string header;
    while(true) {
      auto res = conn.read(&ch, 1);
      if(res.value() <= 0) {
        IRSOL_LOG_ERROR("Failed to read image metadata");
        return std::nullopt;
      }
      if(ch == ':')
        break;
      header += ch;
    }

    IRSOL_LOG_DEBUG("Image header: {}", header);

    // Parse metadata
    int    width = 0, height = 0, channels = 0;
    size_t imageId = 0;
    if(sscanf(header.c_str(), "%lux%dx%dx%d", &imageId, &width, &height, &channels) != 4) {
      IRSOL_LOG_ERROR("Invalid image header: {}", header);
      return std::nullopt;
    }

    int type =
      (channels == 1) ? CV_8UC1 : (channels == 3) ? CV_8UC3 : (channels == 4) ? CV_8UC4 : -1;
    if(type == -1) {
      IRSOL_LOG_ERROR("Unsupported channel count: {}", channels);
      return std::nullopt;
    }

    size_t               expectedSize = width * height * channels;
    std::vector<uint8_t> buffer(expectedSize);

    // Read image bytes
    size_t totalRead = 0;
    while(totalRead < expectedSize) {
      auto res = conn.read(buffer.data() + totalRead, expectedSize - totalRead);
      if(res.value() <= 0) {
        IRSOL_LOG_ERROR("Failed to read image data (read {} of {})", totalRead, expectedSize);
        return std::nullopt;
      }
      totalRead += res.value();
    }

    retrieveTimes.push_back(irsol::types::clock_t::now());
    cv::Mat img = createMat(buffer.data(), height, width, channels);
    result.emplace_back(imageId, std::move(img));
  }

  auto t1            = irsol::types::clock_t::now();
  auto totalDuration = t1 - t0;
  IRSOL_LOG_INFO(
    "Took {} to retrieve {} frames -> fps {}",
    irsol::utils::durationToString(totalDuration),
    result.size(),
    1000000.0 * result.size() /
      std::chrono::duration_cast<std::chrono::microseconds>(totalDuration).count());
  for(size_t i = 0; i < retrieveTimes.size() - 1; ++i) {
    auto start = retrieveTimes[i];
    auto end   = retrieveTimes[i + 1];
    auto dt    = end - start;
    IRSOL_LOG_INFO(
      "Dts: {} -> fps: {}",
      irsol::utils::durationToString(dt),
      1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(dt).count());
  }
  return result;
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
run(double inFps, uint64_t sequenceLength)
{

  IRSOL_LOG_INFO("TCP client viewer (gis)");

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

  if(!configureGis(conn, inFps, sequenceLength)) {
    std::exit(1);
  }

  bool firstFrame = true;

  // Start listening for incoming images

  IRSOL_LOG_INFO("Starting frame polling with FPS: {}", inFps);

  while(!g_terminate.load()) {
    // --- Query image ---
    auto imagesOpt = queryImages(conn);

    if(!imagesOpt.has_value()) {
      IRSOL_LOG_ERROR("Failed to receive image, waiting 1 second..");
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    auto   images     = std::move(imagesOpt.value());
    size_t imageIndex = 0;
    for(const auto& [imageId, image] : images) {

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
        {0, 255, 0},
        1,
        cv::LINE_AA);

      cv::putText(
        image,
        "ImageIndex: " + std::to_string(imageIndex),
        {20, 140},
        cv::FONT_HERSHEY_COMPLEX,
        1.0,
        {0, 255, 0},
        1,
        cv::LINE_AA);

      // --- Display ---
      cv::imshow("Viewer", image);
      int key = cv::waitKey(300) & 0xFF;
      if(key == 27 || key == 'q') {
        IRSOL_LOG_INFO("Exit requested via keyboard");
        g_terminate.store(true);
      }

      ++imageIndex;
    }

    IRSOL_LOG_INFO("Sleeping prior to sending new gis request");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  IRSOL_LOG_INFO("Exiting viewer");
}

int
main(int argc, char** argv)
{
  irsol::initLogging("log/viewer-client-poll.log");
  irsol::initAssertHandler();

  // Register signal handler
  std::signal(SIGTERM, signalHandler);
  std::signal(SIGINT, signalHandler);  // Also handle Ctrl+C

  args::ArgumentParser      parser("TCP client viewer");
  args::HelpFlag            help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<double>   listen_fps_flag(parser, "fps", "The FPS at which to listen.", {'f'});
  args::ValueFlag<uint64_t> isl_flag(
    parser, "isl", "The input sequence length at which to listen.", {'i'});
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
  uint64_t isl = 10;
  if(isl_flag) {
    isl = args::get(isl_flag);
  }

  run(listen_fps, isl);
}