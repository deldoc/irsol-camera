#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {
class CommandAbortHandler : public CommandHandler
{
public:
  CommandAbortHandler(Context ctx);

protected:
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    irsol::protocol::Command&&                    message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol