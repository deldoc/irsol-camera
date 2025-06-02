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
}  // namespace handlers
}  // namespace server
}  // namespace irsol