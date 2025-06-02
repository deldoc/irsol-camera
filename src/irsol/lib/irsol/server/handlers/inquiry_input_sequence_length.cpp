#include "irsol/server/handlers/inquiry_input_sequence_length.hpp"

#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
InquiryInputSequenceLength::InquiryInputSequenceLength(Context ctx): InquiryHandler(ctx) {}

std::vector<out_message_t>
InquiryInputSequenceLength::operator()(
  const irsol::types::client_id_t& clientId,
  IRSOL_MAYBE_UNUSED protocol::Inquiry&& message)
{
  // Retrieve the current session using the client ID
  auto session = this->ctx.getSession(clientId);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", clientId);
    return {};
  }
  auto& frameListeningState = session->userData().frameListeningState;
  auto  inputSequenceLength = frameListeningState.gisParams.inputSequenceLength;

  std::vector<out_message_t> result;
  result.emplace_back(irsol::protocol::Success::from(
    std::move(message), irsol::types::protocol_value_t{static_cast<int>(inputSequenceLength)}));
  return result;
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol