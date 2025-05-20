#include "irsol/server/command_processor.hpp"
#include "irsol/logging.hpp"
#include <memory>
#include <sstream>

namespace irsol {
namespace internal {

std::vector<CommandResponse> CommandProcessor::handleQuery(const std::string &query,
                                                           ClientSession &session) {
  auto &camera = session.app().camera();

  if (query == "camera_status") {
    IRSOL_LOG_DEBUG("Querying camera status");
    auto fps = camera.getParam<float>("AcquisitionFrameRate");
    return {
        CommandResponse{"fr=" + std::to_string(fps) + "\n"},
    };
  }
  if (query == "fps") {
    IRSOL_LOG_DEBUG("Querying frame rate");
    auto fps = camera.getParam<float>("AcquisitionFrameRate");
    return {CommandResponse{"fr=" + std::to_string(fps) + "\n"}};
  }

  if (query == "image") {
    IRSOL_LOG_DEBUG("Querying image");
    auto img = camera.captureImage(std::chrono::milliseconds(10000));
    if (img.IsEmpty()) {
      IRSOL_LOG_ERROR("Failed to capture image.");
      return {};
    }
    int imageId = img.GetImageID();
    int width = img.GetWidth();
    int height = img.GetHeight();
    size_t dataSize = img.GetSize();

    // Create header
    std::ostringstream header;
    header << "image_data:" << width << "x" << height << "x" << 1 << ":";
    std::string headerStr = header.str();

    // Send raw data
    const void *imageBuffer = img.GetImageData();
    void *rawBuffer = new char[dataSize];
    memcpy(rawBuffer, imageBuffer, dataSize);

    // Create shared_ptr with custom deleter to delete raw buffer
    IRSOL_LOG_TRACE("Created image {}, shape ({}x{})", imageId, height, width);
    std::shared_ptr<void> buffer(rawBuffer, [imageId](void *p) {
      IRSOL_LOG_TRACE("Deleting image {}", imageId);
      delete[] static_cast<char *>(p);
    });
    internal::BinaryData binaryData{buffer, dataSize};
    return {
        CommandResponse{headerStr, binaryData},
    };
  }

  IRSOL_LOG_ERROR("Unknown query: '{}'", query);
  return {};
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
    return {CommandResponse{"", {}, "fr=" + params + "\n"}};
  }

  IRSOL_LOG_ERROR("Unknown command: '{}'", command);
  return {};
}
} // namespace internal
} // namespace irsol