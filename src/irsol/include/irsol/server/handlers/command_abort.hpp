#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {
struct CommandAbortHandler : CommandHandler
{
  CommandAbortHandler(Context ctx);

  std::vector<out_message_t> operator()(
    const irsol::types::client_id_t& clientId,
    irsol::protocol::Command&&       message);
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol