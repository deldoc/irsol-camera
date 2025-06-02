#include "irsol/server/handlers/command_gi.hpp"

#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/server/client/session.hpp"

#include <sstream>

namespace irsol {
namespace server {
namespace handlers {

CommandGIHandler::CommandGIHandler(Context ctx): internal::CommandGIBaseHandler(ctx) {}

std::vector<irsol::protocol::OutMessage>
CommandGIHandler::validate(
  IRSOL_MAYBE_UNUSED const protocol::Command& message,
  IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  // For 'gi' command, no further validation to do.
  return {};
}

uint64_t
CommandGIHandler::getInputSequenceLength(
  IRSOL_MAYBE_UNUSED const protocol::Command& message,
  IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  // For 'gi' command, we always capture only 1 frame at the time
  return 1;
}
double
CommandGIHandler::getFrameRate(
  IRSOL_MAYBE_UNUSED const protocol::Command& message,
  IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::internal::ClientSession> session) const
{
  // for 'gi' command we mock the FPS to -1, so that the FrameCollector knows we're only
  // interested in 1 frame
  return -1.0;
}

}  // namespace handlers
}  // namespace server
}  // namespace irsol
