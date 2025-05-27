#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct CommandGIHandler : CommandHandler
{
  CommandGIHandler(Context ctx);

  std::vector<out_message_t> operator()(
    const ::irsol::server::client_id_t& client_id,
    protocol::Command&&                 message) override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol