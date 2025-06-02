#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {
struct CommandAbortHandler : CommandHandler
{
  CommandAbortHandler(Context ctx);

  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    irsol::protocol::Command&&                              message);
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol