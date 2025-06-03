#pragma once

#include "irsol/types.hpp"

#include <memory>

namespace irsol {
namespace server {
// Forward declaration
class App;
namespace internal {
class ClientSession;
}
namespace handlers {
struct Context
{
  App& app;

  /// Utility method to retrieve the client's session associated with the clientId passed as
  /// argument.
  std::shared_ptr<irsol::server::internal::ClientSession> getSession(
    const irsol::types::client_id_t& clientId);

  /// Utility method to broadcast a message to all clients. Optionally disable the broadcasting to a
  /// specific client.
  void broadcastMessage(
    protocol::OutMessage&&                          message,
    const std::optional<irsol::types::client_id_t>& excludeClient = std::nullopt);
};
}
}
}