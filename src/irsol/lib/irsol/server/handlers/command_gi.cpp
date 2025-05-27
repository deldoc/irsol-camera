#include "irsol/server/handlers/command_gi.hpp"

#include "irsol/logging.hpp"
#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/image_data.hpp"

#include <mutex>
#include <vector>
namespace irsol {
namespace server {
namespace handlers {
CommandGIHandler::CommandGIHandler(Context ctx): CommandHandler(ctx) {}

std::vector<out_message_t>
CommandGIHandler::operator()(
  const ::irsol::types::client_id_t& client_id,
  IRSOL_MAYBE_UNUSED protocol::Command&& message)
{
  // Retrieve the current session using the client ID
  auto session = this->ctx.getSession(client_id);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", client_id);
    return {};
  }

  // Register the current client in the frame collector
  auto& collector = this->ctx.app.frameCollector();
  // Set a mock frame-rate to the current session's state to allow the collector to run
  const double frameRate                                = 10;         // Mock frame rate
  session->sessionData().frameListeningParams.frameRate = frameRate;  // Mock frame rate
  session->sessionData().frameListeningParams.lastFrameSent =
    std::chrono::steady_clock::now() -
    std::chrono::microseconds(
      static_cast<uint64_t>(1000000 / frameRate));  // Mock last frame sent so that it looks l
  session->sessionData().frameListeningParams.numDesiredFrames = 1;

  collector.addClient(
    session, [session, &collector](::irsol::server::internal::ImageData imageData) {
      IRSOL_NAMED_LOG_INFO(session->id(), "Frame received from collector");

      // Copy the image data to a byte vector
      std::vector<irsol::types::byte_t> frameBytes = imageData.data;
      irsol::protocol::ImageBinaryData  frameData(
        std::move(frameBytes),
        {imageData.height, imageData.width},
        {irsol::protocol::BinaryDataAttribute("imageId", static_cast<int>(imageData.imageId)),
         irsol::protocol::BinaryDataAttribute(
           "timestamp", std::to_string(imageData.timestamp.time_since_epoch().count()))});

      IRSOL_NAMED_LOG_DEBUG(session->id(), "Sending frame data to client {}", frameData.toString());

      // Send the frame data to the client
      session->handleOutMessage(irsol::protocol::OutMessage(std::move(frameData)));
    });

  return {};
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol