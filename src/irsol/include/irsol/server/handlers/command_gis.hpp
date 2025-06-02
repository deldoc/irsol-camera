#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct CommandGISHandler : CommandHandler
{
  CommandGISHandler(Context ctx);

  std::vector<out_message_t> operator()(
    const ::irsol::types::client_id_t& clientId,
    protocol::Command&&                message) override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol