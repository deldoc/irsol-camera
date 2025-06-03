#include "irsol/server/handlers/base.hpp"

#include "irsol/server/app.hpp"

namespace irsol {
namespace server {
namespace handlers {
std::shared_ptr<irsol::server::internal::ClientSession>
Context::getSession(const irsol::types::client_id_t& clientId)
{
  return app.getClientSession(clientId);
}

void
Context::broadcastMessage(
  irsol::protocol::OutMessage&&                   message,
  const std::optional<irsol::types::client_id_t>& excludeClient)
{
  return app.broadcastMessage(std::move(message), excludeClient);
}
}  // namespace handlers
}  // namespace server
}  // namespace irsol