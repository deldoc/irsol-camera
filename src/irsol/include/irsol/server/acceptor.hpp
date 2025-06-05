/**
 * @file irsol/server/acceptor.hpp
 * @brief Accepts and forwards incoming client connections to the session handler.
 * @ingroup Server
 *
 * This header defines the @ref irsol::server::internal::ClientSessionAcceptor class, responsible
 * for listening on a TCP port and invoking a callback for each new client connection. Typically,
 * this is used to bootstrap a client session when a connection is made.
 *
 * @see irsol::server::App
 * @see irsol::server::ClientSession
 */

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
 * @brief Callback function for handling new client connections.
 * @ingroup Server
 *
 * This callback is invoked by the acceptor when a new TCP client connects.
 * @note Usually this callback is provided by the server @ref irsol::server::App, where clients are
 * then registered and server within a newly spawned thread.
 *
 * @param clientId Unique identifier for the client.
 * @param sock TCP socket for the new client. Ownership is transferred via rvalue reference.
 */
using OnNewClientCallback_t =
  std::function<void(irsol::types::client_id_t, irsol::types::socket_t&&)>;

/**
 * @brief Accepts incoming TCP connections and triggers a callback for each new client.
 * @ingroup Server
 *
 * The ClientSessionAcceptor listens on a given TCP port and forwards each accepted
 * socket to the provided callback, enabling session initialization for each client.
 */
class ClientSessionAcceptor
{
public:
  /**
   * @brief Constructs the acceptor.
   * @param port TCP port to bind and listen on.
   * @param onNewClientCallback Function to call when a new client connects.
   */
  ClientSessionAcceptor(irsol::types::port_t port, OnNewClientCallback_t onNewClientCallback);

  /**
   * @brief Starts the accept loop.
   *
   * This method enters a blocking loop accepting client connections and invoking
   * the callback for each. It is typically run in a dedicated thread.
   */
  void run();

  /**
   * @brief Stops the accept loop.
   *
   * Calling this method will cause `run()` to return and stop accepting new connections.
   */
  void stop();

  /**
   * @brief Gets the current error message, if any.
   * @return A string describing the most recent error that might have been raised while trying to
   * bind to the TCP port.
   * @throws irsol::AssertionException in case the acceptor it not in error mode.
   */
  std::string error() const;

  /**
   * @brief Checks if the acceptor is actively listening.
   * @return True if the socket is open and accepting connections.
   */
  bool isOpen() const;

private:
  /**
   * @brief Flag indicating whether the acceptor should continue running.
   * @note This is usually set by the @ref irsol::server::App (or the main thread that manages the
   * acceptor thread) for shut-down operations.
   */
  std::atomic<bool> m_running{false};

  /**
   * @brief Port on which to accept incoming TCP connections.
   */
  const irsol::types::port_t m_port;

  /**
   * @brief Callback to handle accepted client connections.
   */
  OnNewClientCallback_t m_onNewClientCallback;

  /**
   * @brief TCP acceptor socket bound to m_port.
   */
  irsol::types::acceptor_t m_acceptor;

  /**
   * @brief Result of attempting to bind and open the acceptor socket.
   */
  irsol::types::connection_result_t m_isOpen{};
};

}  // namespace internal
}  // namespace server
}  // namespace irsol
