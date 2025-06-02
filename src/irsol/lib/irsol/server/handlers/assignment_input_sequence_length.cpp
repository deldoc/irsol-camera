#include "irsol/server/handlers/assignment_input_sequence_length.hpp"

#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
AssignmentInputSequenceLength::AssignmentInputSequenceLength(Context ctx): AssignmentHandler(ctx) {}

std::vector<out_message_t>
AssignmentInputSequenceLength::operator()(
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
      session->id(), "Session is already listening to frames. Cannot set a inputSequenceLength.");
    std::vector<out_message_t> result;
    result.emplace_back(irsol::protocol::Error::from(
      message, "Session is already listening to frames. Cannot set a inputSequenceLength."));
    return result;
  }
  const int inputSequenceLength = irsol::utils::toInt(message.value);
  if(inputSequenceLength <= 0) {
    IRSOL_NAMED_LOG_WARN(
      session->id(), "inputSequenceLength ({}) must be positive.", inputSequenceLength);
    std::vector<out_message_t> result;
    result.emplace_back(
      irsol::protocol::Error::from(message, "InputSequenceLength must be positive."));
    return result;
  }
  IRSOL_NAMED_LOG_INFO("Setting 'inputSequenceLength' to {}", inputSequenceLength);

  frameListeningState.gisParams.inputSequenceLength = static_cast<uint64_t>(inputSequenceLength);
  std::vector<out_message_t> result;
  result.emplace_back(irsol::protocol::Success::from(std::move(message)));
  return result;
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol