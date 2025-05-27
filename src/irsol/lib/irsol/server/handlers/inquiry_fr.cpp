#include "irsol/server/handlers/inquiry_fr.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/app.hpp"

namespace irsol {
namespace server {
namespace handlers {

InquiryFrameRateHandler::InquiryFrameRateHandler(Context ctx): InquiryHandler(ctx) {}

std::vector<out_message_t>
InquiryFrameRateHandler::operator()(
  const ::irsol::server::client_id_t& client_id,
  protocol::Inquiry&&                 message)
{
  // Implement logic to calculate and return the frame rate
  // ...
  IRSOL_LOG_INFO("Inquiry frame rate handler called");
  auto& cam = ctx.app.camera();
  auto  fps = cam.getParam<float>("AcquisitionFrameRate");

  std::vector<out_message_t> result;
  result.emplace_back(protocol::Success::from(std::move(message), fps));
  return result;
}

}  // namespace handlers
}  // namespace server
}  // namespace irsol