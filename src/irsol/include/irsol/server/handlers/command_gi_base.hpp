#pragma once

#include "irsol/server/handlers/base.hpp"
#include "irsol/server/image_collector.hpp"

namespace irsol {
namespace server {
namespace handlers {
namespace internal {
struct CommandGIBaseHandler : CommandHandler
{
  CommandGIBaseHandler(Context ctx);

  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    protocol::Command&&                                     message) override;

private:
  std::string getDescription(
    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const;
  /// Override to validate parameters before starting listener thread.
  /// Return vector of protocol::Error messages (empty if no error).
  virtual std::vector<irsol::protocol::OutMessage> validate(
    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const = 0;
  /// Retrieves a description for the current execution of the command
  /// Retrieves the input sequence length to use to start the listening
  virtual uint64_t getInputSequenceLength(
    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const = 0;
  /// Retrieves the frame rate to use to start the listening
  virtual double getFrameRate(
    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const = 0;

  void startListeningThread(
    std::shared_ptr<irsol::server::internal::ClientSession>                        session,
    std::shared_ptr<irsol::server::frame_collector::FrameCollector::frame_queue_t> queue,
    protocol::Command&&                                                            command,
    const std::string&                                                             description);
};
}  // namespace internal
}  // namespace handlers
}  // namespace server
}  // namespace irsol