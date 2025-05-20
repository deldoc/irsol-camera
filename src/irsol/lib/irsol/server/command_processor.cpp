#include "irsol/server/command_processor.hpp"
#include "irsol/logging.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/image_data.hpp"
#include "irsol/utils.hpp"
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
  auto &frameCollector = session.app().frameCollector();

  IRSOL_LOG_DEBUG("Processing command: '{}'", command);

  if (command == "start_frame_listening") {
    session.sessionData().frameListeningParams.active.store(true);
    double fps = std::stod(params);
    session.sessionData().frameListeningParams.frameRate = fps;

    // Register this client as a listener for frame data
    auto sessionPtr = std::shared_ptr<ClientSession>(&session, [](ClientSession *) {
      // no-op deleter: do nothing
    });
    frameCollector.addClient(sessionPtr, [sessionPtr](ImageData imageData) {
      IRSOL_NAMED_LOG_INFO(sessionPtr->id(), "Received image-buffer at tstamp {}",
                           utils::timestamp_to_str(imageData.timestamp));

      // Create header
      std::ostringstream header;
      header << "image_data:" << imageData.width << "x" << imageData.height << "x"
             << imageData.channels << ":";
      std::string headerStr = header.str();

      internal::BinaryData binaryData{imageData.data, imageData.size};
      {
        std::lock_guard<std::mutex> lock(sessionPtr->sessionData().mutex);
        sessionPtr->send(headerStr);
        sessionPtr->send(binaryData.data.get(), binaryData.size);
      }
    });
    IRSOL_LOG_INFO("Started frame listening for {} fps", fps);
    return {};
  }
  if (command == "stop_frame_listening") {
    // Remove this client from the list of listeners for frame data
    session.sessionData().frameListeningParams.active.store(false);
    auto sessionPtr = std::shared_ptr<ClientSession>(&session, [](ClientSession *) {
      // no-op deleter: do nothing
    });
    frameCollector.removeClient(sessionPtr);
    IRSOL_LOG_INFO("Stopped frame listening");
    return {};
  }

  IRSOL_LOG_ERROR("Unknown command: '{}'", command);
  return {};
}
} // namespace internal
} // namespace irsol