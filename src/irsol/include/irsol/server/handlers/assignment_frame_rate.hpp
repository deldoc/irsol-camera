#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

class AssignmentFrameRateHandler : public AssignmentHandler
{
public:
  AssignmentFrameRateHandler(Context ctx);

protected:
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    protocol::Assignment&&                                  message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol