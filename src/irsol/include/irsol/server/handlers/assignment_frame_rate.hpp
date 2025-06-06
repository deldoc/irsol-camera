/**
 * @file irsol/server/handlers/assignment_frame_rate.hpp
 * @brief Declaration of the AssignmentFrameRateHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::AssignmentFrameRateHandler class,
 * which handles assignment messages to set the frame rate for a client session.
 */

#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for assignment of the frame rate parameter.
 * @ingroup Handlers
 *
 * Processes assignment messages to set the frame rate for frame collection in a client session.
 */
class AssignmentFrameRateHandler : public AssignmentHandler
{
public:
  /**
   * @brief Constructs the AssignmentFrameRateHandler.
   * @param ctx Handler context.
   */
  AssignmentFrameRateHandler(Context ctx);

protected:
  /**
   * @brief Processes an assignment message to set the frame rate.
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