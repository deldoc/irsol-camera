#include "irsol/server/handlers/assignment_integration_time.hpp"

#include "irsol/server/app.hpp"
#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace handlers {
AssignmentIntegrationTimeHandler::AssignmentIntegrationTimeHandler(Context ctx)
  : AssignmentHandler(ctx)
{}

std::vector<out_message_t>
AssignmentIntegrationTimeHandler::process(
  std::shared_ptr<irsol::server::ClientSession> session,
  protocol::Assignment&&                        message)
{
  // Try to set the integration time (exposure) according to the message
  if(!message.hasInt() && !message.hasDouble()) {
    IRSOL_NAMED_LOG_ERROR(
      session->id(),
      "Expected integer/double for integration time, got type {}: '{}'",
      message.hasString() ? "<string>" : "<unknown>",
      message.toString());
    return {};
  }
  uint64_t integrationTimeMs;
  if(message.hasInt()) {
    integrationTimeMs = irsol::utils::toInt(message.value);
  } else {
    IRSOL_ASSERT_ERROR(message.hasDouble());
    integrationTimeMs = static_cast<uint64_t>(irsol::utils::toDouble(message.value));
  }
  irsol::types::duration_t integrationTime = std::chrono::microseconds(integrationTimeMs * 1000);
  IRSOL_NAMED_LOG_INFO(
    session->id(),
    "Setting camera integration time to {}",
    irsol::utils::durationToString(integrationTime));
  auto& cam               = session->app().camera();
  auto  resultingExposure = cam.setExposure(integrationTime);

  int resultingExposureMs = static_cast<int>(
    1.0 * std::chrono::duration_cast<std::chrono::microseconds>(resultingExposure).count() / 1000);

  // Broadcast the new integration time to all clients
  ctx.broadcastMessage(irsol::protocol::Success::from(message, {resultingExposureMs}));

  // Don't return anything to the current client, as the client is automatically notified via the
  // above broadcast
  return {};
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol