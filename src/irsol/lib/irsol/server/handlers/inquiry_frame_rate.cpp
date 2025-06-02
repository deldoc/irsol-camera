#include "irsol/server/handlers/inquiry_frame_rate.hpp"

#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/app.hpp"

namespace irsol {
namespace server {
namespace handlers {

InquiryFrameRateHandler::InquiryFrameRateHandler(Context ctx): InquiryHandler(ctx) {}

std::vector<out_message_t>
InquiryFrameRateHandler::operator()(
  IRSOL_MAYBE_UNUSED const ::irsol::types::client_id_t& clientId,
  protocol::Inquiry&&                                   message)
{
  // Retrieve the current session using the client ID
  auto session = this->ctx.getSession(clientId);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", clientId);
    return {};
  }
  auto& frameListeningState = session->sessionData().frameListeningState;
  auto  frameRate           = frameListeningState.gisParams.frameRate;

  std::vector<out_message_t> result;
  result.emplace_back(
    irsol::protocol::Success::from(std::move(message), irsol::types::protocol_value_t{frameRate}));
  return result;
}

}  // namespace handlers
}  // namespace server
}  // namespace irsol