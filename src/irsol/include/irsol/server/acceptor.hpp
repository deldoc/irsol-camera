#pragma once

#include "irsol/server/client/session.hpp"
#include "irsol/types.hpp"

#include <atomic>
#include <functional>
#include <memory>

namespace irsol {
namespace server {
namespace internal {

/**
 * @brief Callback function for when a new client connection is established.
 * @param clientId Unique identifier for the new client.
 * @param sock  TCP socket for the new client. This is a move-only type.
 */
using OnNewClientCallback_t =
  std::function<void(irsol::types::client_id_t, irsol::types::socket_t&&)>;

/**
 * @brief Accepts incoming TCP connections and creates forwards the underlying connection to the
 * callback.
 */
class ClientSessionAcceptor
{
public:
  /**
   * @brief Constructs the acceptor for a given port and onNewClientCallback.
   * @param port TCP port to listen on for client connections.
   * @param onNewClientCallback Callback function to be invoked when a new client connection is
   * established.
   */
  ClientSessionAcceptor(irsol::types::port_t port, OnNewClientCallback_t onNewClientCallback);

  /**
   * @brief Starts the accept loop. This is usually called in a separate thread.
   */
  void run();

  /**
   * @brief Stops the accept loop.
   */
  void stop();

  /**
   * @brief Returns the error message if the acceptor is in error mode.
   */
  std::string error() const;

  /**
   * @brief Returns true if the acceptor is currently open.
   */
  bool isOpen() const;

private:
  /// Atomic flag controlling the acceptor's running state.
  std::atomic<bool> m_running{false};

  /// TCP port on which the acceptor listens for incoming connections.
  const irsol::types::port_t m_port;

  /// Callback function to be invoked when a new client connection is established.
  OnNewClientCallback_t m_onNewClientCallback;

  /// Acceptor socket bound to m_port.
  irsol::types::acceptor_t m_acceptor;

  /// Flag indicating whether the acceptor is currently open.
  irsol::types::connection_result_t m_isOpen{};
};
}
}
}