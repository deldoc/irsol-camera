#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct AssignmentFrameRateHandler : AssignmentHandler
{
  AssignmentFrameRateHandler(Context ctx);

  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    protocol::Assignment&&                                  message) override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol