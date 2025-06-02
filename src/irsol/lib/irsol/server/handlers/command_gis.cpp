#include "irsol/server/handlers/command_gis.hpp"

#include "irsol/logging.hpp"
#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/image_collector.hpp"
#include "irsol/server/image_data.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <mutex>
#include <vector>
namespace irsol {
namespace server {
namespace handlers {
CommandGISHandler::CommandGISHandler(Context ctx): CommandHandler(ctx) {}

std::vector<out_message_t>
CommandGISHandler::operator()(
  const ::irsol::types::client_id_t& clientId,
  IRSOL_MAYBE_UNUSED protocol::Command&& message)
{
  // Retrieve the current session using the client ID
  auto session = this->ctx.getSession(clientId);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", clientId);
    return {};
  }

  // Register the current client in the frame collector
  auto& collector = this->ctx.app.frameCollector();

  auto& frameListeningState = session->sessionData().frameListeningState;

  if(frameListeningState.running) {
    IRSOL_NAMED_LOG_WARN(
      session->id(), "Session is already listening to frames. Ignoring request.");
    std::vector<out_message_t> result;
    result.emplace_back(
      irsol::protocol::Error::from(message, "Session is already listening to frames"));
    return result;
  }
  if(frameListeningState.gisParams.inputSequenceLength == 0) {
    IRSOL_NAMED_LOG_WARN(session->id(), "Gis inputSequenceLength param is 0, this is not allowed.");
    std::vector<out_message_t> result;
    result.emplace_back(irsol::protocol::Error::from(
      message, "Gis inputSequenceLength param is 0, this is not allowed"));
    return result;
  }
  if(frameListeningState.gisParams.frameRate <= 0) {
    IRSOL_NAMED_LOG_WARN(
      session->id(), "Gis frameRate param is non-positive, this is not allowed.");
    std::vector<out_message_t> result;
    result.emplace_back(irsol::protocol::Error::from(
      message, "Gis frameRate param is non-positive, this is not allowed."));
    return result;
  }

  frameListeningState.running = true;
  auto queue                  = collector.makeQueue();
  frameListeningState.listeningThread =
    std::thread([session, queue, &message, &frameListeningState]() {
      IRSOL_NAMED_LOG_INFO(session->id(), "Inside detached listening thread");
      std::unique_ptr<irsol::server::frame_collector::Frame> framePtr;
      while(!queue->done() && queue->pop(framePtr)) {
        IRSOL_NAMED_LOG_DEBUG(
          session->id(), "Sending frame data to client {}", framePtr->image.toString());
        // Send the frame data to the client
        session->handleOutMessage(irsol::protocol::OutMessage(std::move(framePtr->image)));
      }
      IRSOL_NAMED_LOG_INFO(session->id(), "Command {} finished", message.toString());
      frameListeningState.running = false;
      session->handleOutMessage(irsol::protocol::Success::from(std::move(message)));
    });

  collector.registerClient(
    clientId,
    frameListeningState.gisParams.frameRate,
    queue,
    frameListeningState.gisParams.inputSequenceLength);
  frameListeningState.listeningThread.detach();
  IRSOL_NAMED_LOG_INFO(clientId, "Client registered, thread detached, returning from command");
  return {};
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol