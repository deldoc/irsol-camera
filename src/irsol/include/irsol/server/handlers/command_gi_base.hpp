/**
 * @file irsol/server/handlers/command_gi_base.hpp
 * @brief Declaration of the CommandGIBaseHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::internal::CommandGIBaseHandler class,
 * which provides common logic for handling frame acquisition commands.
 *
 * @see irsol::server::handlers::CommandGIHandler
 * @see irsol::server::handlers::CommandGISHandler
 */

#pragma once

#include "irsol/server/handlers/base.hpp"
#include "irsol/server/image_collector.hpp"

namespace irsol {
namespace server {
namespace handlers {
namespace internal {

/**
 * @brief Base handler for frame acquisition commands (`gi`, `gis`).
 * @ingroup Handlers
 *
 * Provides common logic for starting frame collection and managing client registration.
 *
 * @see irsol::server::handlers::CommandGIHandler
 * @see irsol::server::handlers::CommandGISHandler
 */
class CommandGIBaseHandler : public CommandHandler
{
public:
  /**
   * @brief Constructs the CommandGIBaseHandler.
   * @param ctx Handler context.
   */
  CommandGIBaseHandler(std::shared_ptr<Context> ctx);

protected:
  /**
   * @brief Processes a frame acquisition command, starting the frame collection thread.
   * @param session The client session.
   * @param message The command message.
   * @return Vector of outbound messages (success or error).
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Command&&                           message) final override;

private:
  /**
   * @brief Generates a description string for the command execution.
   * @param message The command message.
   * @param session The client session.
   * @return Description string.
   */
  std::string getDescription(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const;

  /**
   * @brief Validates parameters before starting listener thread.
   * @param message The command message.
   * @param session The client session.
   * @return Vector of protocol::Error messages (empty if no error).
   * @note This method must be implemented by derived classes to provide specific validation logic.
   * @see irsol::server::handlers::CommandGIHandler::validate
   * @see irsol::server::handlers::CommandGISHandler::validate
   */
  virtual std::vector<irsol::protocol::OutMessage> validate(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const = 0;

  /**
   * @brief Retrieves the input sequence length to use to start the listening.
   * @param message The command message.
   * @param session The client session.
   * @return Number of frames to acquire.
   * @note This method must be implemented by derived classes to provide specific sequence length
   * logic.
   * @see irsol::server::handlers::CommandGIHandler::getInputSequenceLength
   * @see irsol::server::handlers::CommandGISHandler::getInputSequenceLength
   */
  virtual uint64_t getInputSequenceLength(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const = 0;

  /**
   * @brief Retrieves the frame rate to use to start the listening.
   * @param message The command message.
   * @param session The client session.
   * @return Frame rate for acquisition.
   * @note This method must be implemented by derived classes to provide specific frame rate logic.
   * @see irsol::server::handlers::CommandGIHandler::getFrameRate
   * @see irsol::server::handlers::CommandGISHandler::getFrameRate
   */
  virtual double getFrameRate(
    const protocol::Command&                      message,
    std::shared_ptr<irsol::server::ClientSession> session) const = 0;

  /**
   * @brief Starts the frame listening thread for the client session.
   * @param session The client session.
   * @param queue The frame queue for collected frames.
   * @param command The command message.
   * @param description Description string for logging.
   * @see irsol::server::internal::FrameListeningState::start
   */
  void startListeningThread(
    std::shared_ptr<irsol::server::ClientSession>                                  session,
    std::shared_ptr<irsol::server::frame_collector::FrameCollector::frame_queue_t> queue,
    protocol::Command&&                                                            command,
    const std::string&                                                             description);
};
}  // namespace internal
}  // namespace handlers
}  // namespace server
}  // namespace irsol