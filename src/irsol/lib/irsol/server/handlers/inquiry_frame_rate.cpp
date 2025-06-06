#include "irsol/server/handlers/inquiry_frame_rate.hpp"

#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/app.hpp"

namespace irsol {
namespace server {
namespace handlers {

InquiryFrameRateHandler::InquiryFrameRateHandler(std::shared_ptr<Context> ctx): InquiryHandler(ctx)
{}

std::vector<out_message_t>
InquiryFrameRateHandler::process(
  IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::ClientSession> session,
  protocol::Inquiry&&                                              message)
{
  auto& frameListeningState = session->userData().frameListeningState;
  auto  frameRate           = frameListeningState.gisParams.frameRate;

  std::vector<out_message_t> result;
  result.emplace_back(
    irsol::protocol::Success::from(message, irsol::types::protocol_value_t{frameRate}));
  return result;
}

}  // namespace handlers
}  // namespace server
}  // namespace irsol