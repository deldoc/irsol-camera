#include "irsol/server/handlers/assignment_frame_rate.hpp"

#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
AssignmentFrameRateHandler::AssignmentFrameRateHandler(Context ctx): AssignmentHandler(ctx) {}

std::vector<out_message_t>
AssignmentFrameRateHandler::operator()(
  const irsol::types::client_id_t& clientId,
  protocol::Assignment&&           message)
{
  // Retrieve the current session using the client ID
  auto session = this->ctx.getSession(clientId);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", clientId);
    return {};
  }
  auto& frameListeningState = session->userData().frameListeningState;
  if(frameListeningState.running()) {
    IRSOL_NAMED_LOG_WARN(
      session->id(), "Session is already listening to frames. Cannot set a frameRate.");
    std::vector<out_message_t> result;
    result.emplace_back(irsol::protocol::Error::from(
      message, "Session is already listening to frames. Cannot set a frameRate."));
    return result;
  }
  const double frameRate = irsol::utils::toDouble(message.value);
  if(frameRate <= 0) {
    IRSOL_NAMED_LOG_WARN(session->id(), "frameRate ({}) must be positive.", frameRate);
    std::vector<out_message_t> result;
    result.emplace_back(irsol::protocol::Error::from(message, "frameRate must be positive."));
    return result;
  }
  IRSOL_NAMED_LOG_INFO("Setting 'frameRate' to {}", frameRate);

  frameListeningState.gisParams.frameRate = frameRate;
  std::vector<out_message_t> result;
  result.emplace_back(irsol::protocol::Success::from(message));
  return result;
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol