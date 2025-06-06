/**
 * @file irsol/server/handlers/assignment_integration_time.hpp
 * @brief Declaration of the AssignmentIntegrationTimeHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::AssignmentIntegrationTimeHandler class,
 * which handles assignment messages to set the camera integration time (exposure).
 */

#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for assignment of the camera integration time (exposure).
 * @ingroup Handlers
 *
 * Processes assignment messages to set the camera's integration time.
 */
class AssignmentIntegrationTimeHandler : public AssignmentHandler
{
public:
  /**
   * @brief Constructs the AssignmentIntegrationTimeHandler.
   * @param ctx Handler context.
   */
  AssignmentIntegrationTimeHandler(std::shared_ptr<Context> ctx);

protected:
  /**
   * @brief Processes an assignment message to set the integration time.
   * @param session The client session.
   * @param message The assignment message.
   * @return Vector of outbound messages (success or error).
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Assignment&&                        message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol