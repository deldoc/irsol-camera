#pragma once

#include "irsol/server/handlers/command_gi_base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct CommandGIHandler : internal::CommandGIBaseHandler
{
  CommandGIHandler(Context ctx);

private:
  std::vector<irsol::protocol::OutMessage> validate(

    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const override;

  uint64_t getInputSequenceLength(
    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const override;
  double getFrameRate(
    const protocol::Command&                                message,
    std::shared_ptr<irsol::server::internal::ClientSession> session) const override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol