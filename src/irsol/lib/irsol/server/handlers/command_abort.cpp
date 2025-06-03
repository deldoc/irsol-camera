#include "irsol/server/handlers/command_abort.hpp"

#include "irsol/macros.hpp"
#include "irsol/server/client/session.hpp"

namespace irsol {
namespace server {
namespace handlers {
CommandAbortHandler::CommandAbortHandler(Context ctx): CommandHandler(ctx) {}

std::vector<out_message_t>
CommandAbortHandler::process(
  std::shared_ptr<irsol::server::ClientSession> session,
  IRSOL_MAYBE_UNUSED irsol::protocol::Command&& message)
{
  auto& state = session->userData().frameListeningState;
  if(!state.running()) {
    IRSOL_NAMED_LOG_INFO(
      session->id(), "Session is not listening to frames. Ignoring abort request.");
    return {};
  }

  IRSOL_NAMED_LOG_INFO(session->id(), "Stopping request set to detached background thread..");
  state.stop();
  std::vector<out_message_t> result;
  result.emplace_back(irsol::protocol::Success::from(message));
  return result;
}

}
}
}