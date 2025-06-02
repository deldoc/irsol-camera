#include "irsol/server/handlers/command_gi_base.hpp"

#include "irsol/logging.hpp"
#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/image_collector.hpp"
#include "irsol/server/image_data.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

namespace irsol {
namespace server {
namespace handlers {
namespace internal {

CommandGIBaseHandler::CommandGIBaseHandler(Context ctx): CommandHandler(ctx) {}

std::vector<out_message_t>
CommandGIBaseHandler::operator()(
  const irsol::types::client_id_t& clientId,
  IRSOL_MAYBE_UNUSED protocol::Command&& message)
{
  // Retrieve the current session using the client ID
  auto session = this->ctx.getSession(clientId);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", clientId);
    return {};
  }

  auto& collector = this->ctx.app.frameCollector();
  auto& state     = session->userData().frameListeningState;

  if(state.running()) {
    IRSOL_NAMED_LOG_WARN(session->id(), "Session already listening to frames. Ignoring request.");
    std::vector<out_message_t> result;
    result.emplace_back(protocol::Error::from(message, "Session is already listening to frames"));
    return result;
  }

  if(auto errors = validate(message, session); errors.size()) {
    return errors;
  }

  auto queue = collector.makeQueuePtr();
  startListeningThread(session, queue, std::move(message), getDescription(message, session));

  const uint64_t numFrames = getInputSequenceLength(message, session);
  const double   fps       = getFrameRate(message, session);
  collector.registerClient(clientId, fps, queue, numFrames);

  IRSOL_NAMED_LOG_INFO(clientId, "Client registered for {} frames at FPS {}", numFrames, fps);
  return {};
}

std::string
CommandGIBaseHandler::getDescription(
  const protocol::Command&                                message,
  std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  std::stringstream ss;
  ss << session->id() << " - " << message.toString();
  return ss.str();
}

void
CommandGIBaseHandler::startListeningThread(
  std::shared_ptr<irsol::server::internal::ClientSession>                        session,
  std::shared_ptr<irsol::server::frame_collector::FrameCollector::frame_queue_t> queue,
  protocol::Command&&                                                            command,
  const std::string&                                                             description)
{
  auto& state = session->userData().frameListeningState;

  state.start(
    [session, queue, message = std::move(command)](
      std::shared_ptr<std::atomic<bool>> stopRequest) mutable {
      // Reset the state of the user-data related to frame-listening
      auto& state                         = session->userData().frameListeningState;
      state.gisParams.inputSequenceNumber = 0;

      IRSOL_NAMED_LOG_INFO(
        session->id(), "Started frame listening thread for {}", message.toString());

      std::unique_ptr<frame_collector::Frame> framePtr;
      bool                                    interrupted = false;
      while(!interrupted && (!queue->done() && queue->pop(framePtr))) {
        IRSOL_NAMED_LOG_DEBUG(
          session->id(),
          "Sending frame {} to client: {}",
          state.gisParams.inputSequenceNumber,
          framePtr->image.toString());
        {
          auto                                     lock = std::scoped_lock(session->socketMutex());
          std::vector<irsol::protocol::OutMessage> result;
          result.emplace_back(irsol::protocol::OutMessage(std::move(framePtr->image)));
          result.emplace_back(irsol::protocol::Success::asStatus(
            "isn", {static_cast<int>(state.gisParams.inputSequenceNumber)}));
          session->handleOutMessages(std::move(result));
        }
        ++state.gisParams.inputSequenceNumber;
        if(stopRequest->load()) {
          interrupted = true;
        }
      }

      if(interrupted) {
        IRSOL_NAMED_LOG_INFO(
          session->id(), "Stopping execution of frame-collection due to stop-request.");
        // TODO: on user-stop requests do we want to return a success?
        return;
      }

      {
        auto lock = std::scoped_lock(session->socketMutex());
        session->handleOutMessage(protocol::Success::from(message));
      }
      IRSOL_NAMED_LOG_INFO(session->id(), "{} frame sending complete", message.toString());
    },
    description);
}
}  // namespace internal
}  // namespace handlers
}  // namespace server
}  // namespace irsol
