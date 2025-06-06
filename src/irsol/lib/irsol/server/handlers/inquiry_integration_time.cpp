#include "irsol/server/handlers/inquiry_integration_time.hpp"

#include "irsol/macros.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
InquiryIntegrationTimeHandler::InquiryIntegrationTimeHandler(std::shared_ptr<Context> ctx)
  : InquiryHandler(ctx)
{}

std::vector<out_message_t>
InquiryIntegrationTimeHandler::process(
  std::shared_ptr<irsol::server::ClientSession> session,
  IRSOL_MAYBE_UNUSED irsol::protocol::Inquiry&& message)
{
  auto& cam      = session->app().camera();
  auto  exposure = cam.getExposure();

  std::vector<out_message_t> result;
  result.emplace_back(irsol::protocol::Success::from(
    message,
    static_cast<int>(
      1.0 * std::chrono::duration_cast<std::chrono::microseconds>(exposure).count() / 1000)));
  return result;
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol