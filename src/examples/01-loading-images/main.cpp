/// @file
/// @brief Example application using the `irsol` library to initialize logging,
/// connect to a camera, and capture a sequence of images.
///
/// This program serves as a minimal working example for new developers to understand
/// how to interact with the core components of the `irsol` library. It demonstrates:
/// - Command-line argument parsing
/// - Logging setup and log level configuration
/// - Camera interface initialization
/// - Repeated image acquisition with basic performance metrics
///
/// Build system integration is expected to define `PROGRAM_NAME` for logging.

#include "irsol/irsol.hpp"

#include <chrono>
#include <unordered_map>

/// @brief Returns the program name, typically used for logging.
/// If `PROGRAM_NAME` is not defined at compile time, returns `"unknown-program"`.
const std::string
getProgramName()
{
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "unknown-program"
#endif
  return PROGRAM_NAME;
}

/// @brief Command-line parameters for the program.
struct Params
{
  /// Logging verbosity level (default: info)
  spdlog::level::level_enum logLevel{spdlog::level::info};

  /// Number of frames to capture from the camera (default: 10)
  uint64_t numFrames{10};

  /// FPS at which to capture frames from the camera (negative means as fast as you can)
  double fps = -1;
};

/// @brief Parses command-line arguments using `args` library.
///
/// Supported arguments:
/// - `--log-level`, `-l`: Set logging level (trace, debug, info, warn, error)
/// - `--frames-count`, `-f`: Set number of frames to capture
/// - `--help`, `-h`: Show help message and exit
///
/// @param argc Argument count
/// @param argv Argument vector
/// @return Filled `Params` struct
Params
getParams(int argc, char** argv)
{
  args::ArgumentParser parser(getProgramName());
  args::HelpFlag       help(parser, "help", "Display this help menu", {'h', "help"});

  args::MapFlag<std::string, spdlog::level::level_enum> logLevelArg(
    parser, "level", "Log level", {'l', "log-level"}, irsol::levelNameToLevelMap);

  args::ValueFlag<uint64_t> numFramesArg(
    parser, "#frames", "Number of frames to iterate over", {'n', "frames-count"});

  args::ValueFlag<double> fpsArg(parser, "fps", "Desired FPS for capturing image", {'f', "fps"});

  try {
    parser.ParseCLI(argc, argv);
  } catch(args::Help) {
    std::cerr << parser.Help() << std::endl;
    std::exit(0);
  } catch(args::ParseError e) {
    std::cerr << "Error parsing command-line arguments:\n" << e.what() << "\n\n" << parser.Help();
    std::exit(1);
  }

  Params params{};
  if(logLevelArg) {
    params.logLevel = args::get(logLevelArg);
  }
  if(numFramesArg) {
    params.numFrames = args::get(numFramesArg);
  }
  if(fpsArg) {
    params.fps = args::get(fpsArg);
  }
  return params;
}

/// @brief Captures a single image using the provided camera interface,
/// logs metadata and performance stats.
///
/// @param cam Reference to an initialized `irsol::camera::Interface` instance.
void
runCapture(irsol::camera::Interface& cam)
{
  auto image = cam.captureImage();

  if(image.IsEmpty()) {
    IRSOL_LOG_WARN("Image is empty.");
  }

  IRSOL_LOG_INFO("{}x{}, {} bytes", image.GetHeight(), image.GetWidth(), image.GetSize());
}

/// @brief Program entrypoint.
///
/// - Initializes logging and assertions
/// - Selects camera resolution based on build mode
/// - Captures a user-defined number of frames
/// - Logs image statistics and performance
int
main(int argc, char** argv)
{
  // Parse command-line parameters
  auto params = getParams(argc, argv);

  // Construct log file path based on program name
  std::string logPath = "logs/" + getProgramName() + ".log";
  irsol::initLogging(logPath.c_str(), params.logLevel);

  // Enable custom assertion handler
  irsol::initAssertHandler();

  // Choose camera resolution based on build type
#ifdef DEBUG
  auto cameraInterface = irsol::camera::Interface::HalfResolution();
#else
  auto cameraInterface = irsol::camera::Interface::FullResolution();
#endif

  // Ensure the camera is connected before continuing
  IRSOL_ASSERT_FATAL(cameraInterface.isConnected(), "Camera is not connected");

  // Log camera metadata for debugging purposes
  IRSOL_LOG_INFO("\n{}", cameraInterface.cameraInfoAsString());
  IRSOL_LOG_INFO("\n{}", cameraInterface.cameraStatusAsString());

  // Capture and log a sequence of frames
  auto       lastTick = irsol::types::clock_t::now();
  const auto desiredDt =
    params.fps > 0.0 ? std::chrono::microseconds(static_cast<uint64_t>(1000000.0 / params.fps))
                     : std::chrono::microseconds(0);
  for(size_t i = 0; i < params.numFrames; ++i) {
    const auto t0 = irsol::types::clock_t::now();
    runCapture(cameraInterface);
    const auto nextTick = lastTick + desiredDt;
    lastTick            = nextTick;
    std::this_thread::sleep_until(nextTick);
    const auto loopDuration = irsol::types::clock_t::now() - t0;
    const auto measuredFps =
      1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(loopDuration).count();
    IRSOL_LOG_INFO("Desired FPS: {:.4f}, measured FPS: {:.4f}", params.fps, measuredFps);
  }

  return 0;
}
