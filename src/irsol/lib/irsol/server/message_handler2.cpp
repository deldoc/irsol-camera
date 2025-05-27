#include "irsol/logging.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/client.hpp"
#include "irsol/server/image_data.hpp"
#include "irsol/server/message_handler.hpp"
#include "irsol/utils.hpp"

#include <memory>
#include <sstream>

namespace irsol {
namespace server {
namespace internal {
MessageHandler::responses_t
MessageHandler::handleQuery(const std::string& query, std::shared_ptr<ClientSession> session)
{
  auto& camera = session->app().camera();

  if(query == "image") {
    IRSOL_LOG_DEBUG("Querying image");
    auto img = camera.captureImage(std::chrono::milliseconds(10000));
    if(img.IsEmpty()) {
      IRSOL_LOG_ERROR("Failed to capture image.");
      return {};
    }
    int    imageId  = img.GetImageID();
    int    width    = img.GetWidth();
    int    height   = img.GetHeight();
    size_t dataSize = img.GetSize();

    // Create header
    std::ostringstream header;
    header << "image_data:" << width << "x" << height << "x" << 1 << ":";
    std::string headerStr = header.str();

    // Send raw data
    const void* imageBuffer = img.GetImageData();
    void*       rawBuffer   = new char[dataSize];
    memcpy(rawBuffer, imageBuffer, dataSize);

    // Create shared_ptr with custom deleter to delete raw buffer
    IRSOL_LOG_TRACE("Created image {}, shape ({}x{})", imageId, height, width);
    std::shared_ptr<void> buffer(rawBuffer, [imageId](void* p) {
      IRSOL_LOG_TRACE("Deleting image {}", imageId);
      delete[] static_cast<char*>(p);
    });
    internal::BinaryData  binaryData{buffer, dataSize};
    return {
      MessageHandlerResponse{headerStr, binaryData},
    };
  }

  IRSOL_LOG_ERROR("Unknown query: '{}'", query);
  return {};
}
MessageHandler::responses_t
MessageHandler::handleCommand(
  const std::string&             command,
  const std::string&             params,
  std::shared_ptr<ClientSession> session)
{
  auto& camera         = session->app().camera();
  auto& frameCollector = session->app().frameCollector();

  IRSOL_LOG_DEBUG("Processing command: '{}'", command);

  if(command == "it") {
    double exposureTime = std::stod(params);
    camera.setParam("ExposureTime", exposureTime);
    return {MessageHandlerResponse{"", {}, "it=" + params + "\n"}};
  }

  if(command == "start_frame_listening") {
    double fps                                            = std::stod(params);
    session->sessionData().frameListeningParams.frameRate = fps;

    frameCollector.addClient(session, [session](ImageData imageData) {
      IRSOL_NAMED_LOG_INFO(
        session->id(),
        "Received image-buffer at tstamp {}",
        utils::timestamp_to_str(imageData.timestamp));

      // Create header
      std::ostringstream header;
      header << "image_data:" << imageData.imageId << "x" << imageData.width << "x"
             << imageData.height << "x" << imageData.channels << ":";
      std::string headerStr = header.str();

      internal::BinaryData binaryData{imageData.data, imageData.size};
      {
        std::lock_guard<std::mutex> lock(session->sessionData().mutex);
        session->send(headerStr);
        session->send(binaryData.data.get(), binaryData.size);
      }
    });
    IRSOL_LOG_INFO("Started frame listening for {} fps", fps);
    return {};
  }
  if(command == "stop_frame_listening") {
    // Remove this client from the list of listeners for frame data
    frameCollector.removeClient(session);
    IRSOL_LOG_INFO("Stopped frame listening");
    return {};
  }

  IRSOL_LOG_ERROR("Unknown command: '{}'", command);
  return {};
}
}  // namespace internal
}  // namespace server
}  // namespace irsol