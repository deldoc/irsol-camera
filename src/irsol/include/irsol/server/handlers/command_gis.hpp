/**
 * @file irsol/server/handlers/command_gis.hpp
 * @brief Declaration of the CommandGISHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::CommandGISHandler class,
 * which handles the `gis` command for sequence frame acquisition.
 */

#pragma once

#include "irsol/server/handlers/command_gi_base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for the `gis` command (sequence frame acquisition).
 * @ingroup Handlers
 *
 * Processes the `gis` command to acquire a sequence of frames from the camera.
 */
class CommandGISHandler : public internal::CommandGIBaseHandler
{
public:
  /**
   * @brief Constructs the CommandGISHandler.
   * @param ctx Handler context.
   */
  CommandGISHandler(std::shared_ptr<Context> ctx);

private:
  /**
   * @brief Validates the `gis` command parameters.
   * @param message The command message.
   * @param session The client session.
   * @return Vector of outbound error messages (empty if valid).
   */
  std::vector<irsol::protocol::OutMessage> validate(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const override;

  /**
   * @brief Returns the input sequence length for the `gis` command.
   * @param message The command message.
   * @param session The client session.
   * @return Number of frames to acquire.
   * @note This method picks the input sequence length from the @ref irsol::server::ClientSession
   * state.
   */
  uint64_t getInputSequenceLength(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const override;

  /**
   * @brief Returns the frame rate for the `gis` command.
   * @param message The command message.
   * @param session The client session.
   * @return Frame rate for acquisition.
   * @note This method picks the frame rate from the @ref irsol::server::ClientSession state.
   */
  double getFrameRate(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol