#include "irsol/server/handlers/inquiry_input_sequence_length.hpp"

#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
InquiryInputSequenceLengthHandler::InquiryInputSequenceLengthHandler(std::shared_ptr<Context> ctx)
  : InquiryHandler(ctx)
{}

std::vector<out_message_t>
InquiryInputSequenceLengthHandler::process(
  std::shared_ptr<irsol::server::ClientSession> session,
  IRSOL_MAYBE_UNUSED protocol::Inquiry&& message)
{

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