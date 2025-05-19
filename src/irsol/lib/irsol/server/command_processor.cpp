#include "irsol/server/command_processor.hpp"
#include "irsol/logging.hpp"

namespace irsol {
namespace internal {

std::vector<CommandResponse> CommandProcessor::handleQuery(const std::string &query,
                                                           ClientSession &session) {
  auto &camera = session.app().camera();

  if (query == "camera_status") {
    IRSOL_LOG_DEBUG("Querying camera status");
    auto fps = camera.getParam<float>("AcquisitionFrameRate");
    return {
        CommandResponse{"fr=" + std::to_string(fps)},
    };
  }
  if (query == "fps") {
    IRSOL_LOG_DEBUG("Querying frame rate");
    auto fps = camera.getParam<float>("AcquisitionFrameRate");
    return {CommandResponse{"fr=" + std::to_string(fps)}};
  }

  IRSOL_LOG_ERROR("Unknown query: '{}'", query);
  return {CommandResponse{"ERR Unknown query"}};
}
std::vector<CommandResponse> CommandProcessor::handleCommand(const std::string &command,
                                                             const std::string &params,
                                                             ClientSession &session) {
  auto &camera = session.app().camera();

  IRSOL_LOG_DEBUG("Processing command: '{}'", command);

  if (command == "fr") {
    // Make sure to enable the ability to set frame rate
    camera.setParam("AcquisitionFrameRateEnable", true);

    auto old_fps = camera.getParam<float>("AcquisitionFrameRate");
    double fps = std::stod(params);
    IRSOL_LOG_INFO("Setting frame rate to {}", fps);
    camera.setParam("AcquisitionFrameRate", fps);
    auto new_fps = camera.getParam<float>("AcquisitionFrameRate");
    IRSOL_LOG_DEBUG("Frame rate updated from {} to {}", old_fps, new_fps);
    return {CommandResponse{"", "fr=" + params}};
  }
  if (command == "CAPTURE") {
    auto img = camera.captureImage();
    // Send dummy reply or image path
    return {CommandResponse{"CAPTURED"}};
  }

  IRSOL_LOG_ERROR("Unknown command: '{}'", command);
  return {CommandResponse{"ERR Unknown command"}};
}
} // namespace internal
} // namespace irsol