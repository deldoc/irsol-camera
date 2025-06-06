/**
 * @file irsol/server/handlers/assignment_input_sequence_length.hpp
 * @brief Declaration of the AssignmentInputSequenceLengthHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::AssignmentInputSequenceLengthHandler class,
 * which handles assignment messages to set the input sequence length for a client session.
 */

#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for assignment of the input sequence length parameter `isl`.
 * @ingroup Handlers
 *
 * Processes assignment messages to set the number of frames to collect in a client session during
 * the `isl` @ref irsol::protocol::Command.
 */
class AssignmentInputSequenceLengthHandler : public AssignmentHandler
{
public:
  /**
   * @brief Constructs the AssignmentInputSequenceLengthHandler.
   * @param ctx Handler context.
   */
  AssignmentInputSequenceLengthHandler(Context ctx);

protected:
  /**
   * @brief Processes an assignment message to set the input sequence length.
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