#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct AssignmentInputSequenceLength : AssignmentHandler
{
  AssignmentInputSequenceLength(Context ctx);

  std::vector<out_message_t> operator()(
    const ::irsol::types::client_id_t& clientId,
    protocol::Assignment&&             message) override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol