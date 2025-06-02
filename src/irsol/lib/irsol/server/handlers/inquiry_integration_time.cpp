#include "irsol/server/handlers/inquiry_integration_time.hpp"

#include "irsol/macros.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
InquiryIntegrationTimeHandler::InquiryIntegrationTimeHandler(Context ctx): InquiryHandler(ctx) {}

std::vector<out_message_t>
InquiryIntegrationTimeHandler::operator()(
  const irsol::types::client_id_t& clientId,
  IRSOL_MAYBE_UNUSED irsol::protocol::Inquiry&& message)
{
  auto session = ctx.getSession(clientId);
  if(!session) {
    IRSOL_LOG_ERROR("No session found for client {}", clientId);
    return {};
  }

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