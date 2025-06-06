/**
 * @file irsol/server/handlers/command_gi.hpp
 * @brief Declaration of the CommandGIHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::CommandGIHandler class,
 * which handles the `gi` command for single frame acquisition.
 */

#pragma once

#include "irsol/server/handlers/command_gi_base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for the `gi` command (single frame acquisition).
 * @ingroup Handlers
 *
 * Processes the `gi` command to acquire a single frame from the camera.
 */
class CommandGIHandler : public internal::CommandGIBaseHandler
{
public:
  /**
   * @brief Constructs the CommandGIHandler.
   * @param ctx Handler context.
   */
  CommandGIHandler(Context ctx);

private:
  /**
   * @brief Validates the `gi` command parameters.
   * @param message The command message.
   * @param session The client session.
   * @return Vector of outbound error messages (empty if valid).
   */
  std::vector<irsol::protocol::OutMessage> validate(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const override;

  /**
   * @brief Returns the input sequence length for the `gi` command (always 1).
   * @param message The command message.
   * @param session The client session.
   * @return Number of frames to acquire (always 1).
   */
  uint64_t getInputSequenceLength(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const override;

  /**
   * @brief Returns the frame rate for the `gi` command.
   * @param message The command message.
   * @param session The client session.
   * @return Frame rate (always -1.0 for single frame).
   */
  double getFrameRate(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol