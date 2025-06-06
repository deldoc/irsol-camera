/**
 * @file irsol/server/handlers/context.hpp
 * @brief Declaration of the handler context struct.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::Context struct, which provides
 * utility methods and access to the application instance for handler classes.
 *
 * This context is passed to all handler classes and provides methods to
 * retrieve client sessions and broadcast messages to clients.
 */

#pragma once

#include "irsol/types.hpp"

#include <memory>

namespace irsol {
namespace server {
// Forward declaration
class App;
class ClientSession;

namespace handlers {

/**
 * @brief Context object passed to all handler classes.
 * @ingroup Handlers
 *
 * Provides access to the application instance and utility methods for
 * retrieving client sessions and broadcasting messages.
 */
struct Context
{
  /// Reference to the application instance.
  App& app;

  /**
   * @brief Retrieves the client's session associated with the given clientId.
   * @param clientId The client identifier.
   * @return Shared pointer to the client session, or nullptr if not found.
   */
  std::shared_ptr<irsol::server::ClientSession> getSession(
    const irsol::types::client_id_t& clientId);

  /**
   * @brief Broadcasts a message to all clients.
   * @param message The outbound message to broadcast.
   * @param excludeClient Optional client to exclude from the broadcast.
   */
  void broadcastMessage(
    protocol::OutMessage&&                          message,
    const std::optional<irsol::types::client_id_t>& excludeClient = std::nullopt);
};
}
}
}