/**
 * @file irsol/server/handlers/command_abort.hpp
 * @brief Declaration of the CommandAbortHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::CommandAbortHandler class,
 * which handles `abort` @ref irsol::protocol::Command from clients.
 */

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for aborting ongoing operations for a client session.
 * @ingroup Handlers
 *
 * Processes abort commands to stop frame collection `gis`.
 */
class CommandAbortHandler : public CommandHandler
{
public:
  /**
   * @brief Constructs the CommandAbortHandler.
   * @param ctx Handler context.
   */
  CommandAbortHandler(Context ctx);

protected:
  /**
   * @brief Processes an abort command.
   * @param session The client session.
   * @param message The abort command message.
   * @return Vector of outbound messages (success or error).
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    irsol::protocol::Command&&                    message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol