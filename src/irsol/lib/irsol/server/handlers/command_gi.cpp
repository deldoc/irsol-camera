#include "irsol/server/handlers/command_gi.hpp"

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
CommandGIHandler::CommandGIHandler(Context ctx): CommandHandler(ctx) {}

std::vector<out_message_t>
CommandGIHandler::operator()(
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

  // We only take a snapshot, so we request a single frame.
  const uint64_t frameCount = 1;
  const double   dummyFps   = -1.0;
  collector.registerClient(clientId, dummyFps, queue, frameCount);
  frameListeningState.listeningThread.detach();
  IRSOL_NAMED_LOG_INFO(clientId, "Client registered, thread detached, returning from command");
  return {};
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol