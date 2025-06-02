#include "irsol/server/handlers/command_gis.hpp"

#include "irsol/protocol.hpp"
#include "irsol/server/client/session.hpp"

namespace irsol {
namespace server {
namespace handlers {
CommandGISHandler::CommandGISHandler(Context ctx): internal::CommandGIBaseHandler(ctx) {}

std::vector<irsol::protocol::OutMessage>
CommandGISHandler::validate(
  const protocol::Command&                                message,
  std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  auto&                      state = session->userData().frameListeningState;
  std::vector<out_message_t> result;
  // For 'gi' command, no further validation to do.
  if(state.gisParams.inputSequenceLength == 0) {
    IRSOL_NAMED_LOG_WARN(session->id(), "Gis inputSequenceLength param is 0, this is not allowed.");
    result.emplace_back(irsol::protocol::Error::from(
      message, "Gis inputSequenceLength param is 0, this is not allowed"));
    return result;
  }
  if(state.gisParams.frameRate <= 0) {
    IRSOL_NAMED_LOG_WARN(
      session->id(), "Gis frameRate param is non-positive, this is not allowed.");
    result.emplace_back(irsol::protocol::Error::from(
      message, "Gis frameRate param is non-positive, this is not allowed."));
    return result;
  }
  return result;
}

uint64_t
CommandGISHandler::getInputSequenceLength(
  IRSOL_MAYBE_UNUSED const protocol::Command&             message,
  std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  const auto& state = session->userData().frameListeningState;
  return state.gisParams.inputSequenceLength;
}
double
CommandGISHandler::getFrameRate(
  IRSOL_MAYBE_UNUSED const protocol::Command&             message,
  std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  const auto& state = session->userData().frameListeningState;
  return state.gisParams.frameRate;
}

}  // namespace handlers
}  // namespace server
}  // namespace irsol