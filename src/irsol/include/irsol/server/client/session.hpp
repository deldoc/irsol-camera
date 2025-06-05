/**
 * @file irsol/server/client/session.hpp
 * @brief Declaration of the `ClientSession` class.
 * @ingroup Server
 *
 * This file defines the @ref irsol::server::ClientSession class, which encapsulates all logic
 * related to managing communication and state for a single connected client in the system.
 */

#pragma once

#include "irsol/protocol.hpp"
#include "irsol/server/client/state.hpp"
#include "irsol/types.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace irsol {
namespace server {

// Forward declaration
class App;

/**
 * @brief Represents a single connected client session.
 * @ingroup Server
 *
 * Each instance of `ClientSession` corresponds to **exactly one connected client**.
 * It owns the socket connection, manages message transmission and reception,
 * and encapsulates session-specific state (e.g., frame stream preferences).
 *
 * The class is responsible for all communication with the client: reading
 * inbound messages from the socket, parsing and dispatching them, and sending
 * outbound messages. All session-specific state is stored here, including a
 * @ref irsol::server::internal::UserSessionData structure.
 *
 * In addition, the session holds a reference to the central @ref irsol::server::App instance,
 * which enables it to interact with the global server context (e.g., for broadcasting).
 *
 * The class derives from `std::enable_shared_from_this` to allow safe
 * creation of `std::shared_ptr` instances to itself. This is required for
 * asynchronous operations and thread-safe lifetime management.
 */
class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
  /**
   * @brief Constructs a new ClientSession.
   *
   * @param id    Unique client identifier (e.g., UUID or socket-derived).
   * @param sock  Already-established TCP socket to the client.
   * @param app   Reference to the server application instance.
   */
  ClientSession(const irsol::types::client_id_t& id, irsol::types::socket_t&& sock, App& app);

  /**
   * @brief Starts the client's session logic.
   *
   * Initiates the processing loop for the session, including reading
   * incoming messages from the socket and handling disconnections.
   */
  void run();

  /**
   * @brief Handles multiple outbound messages to the client.
   *
   * Serializes and sends a vector of outbound messages through the socket.
   *
   * @note This method is not thread-safe; users must ensure mutual exclusion externally.
   *
   * @see irsol::protocol::Serializer
   * @see ClientSession::send
   *
   * @param messages The list of messages to transmit.
   */
  void handleOutMessages(std::vector<protocol::OutMessage>&& messages);

  /**
   * @brief Handles a single outbound message to the client.
   *
   * Serializes and sends one message through the socket.
   *
   * @note This method is not thread-safe; users must ensure mutual exclusion externally.
   *
   * @see irsol::protocol::Serializer
   * @see ClientSession::send
   *
   * @param message The message to transmit.
   */
  void handleOutMessage(protocol::OutMessage&& message);

  /**
   * @brief Sends an already-serialized message to the client.
   *
   * @note This method is not thread-safe; users must ensure mutual exclusion externally.
   *
   * @see irsol::protocol::Serializer
   * @see ClientSession::send
   *
   * @param serializedMessage Message that is already serialized to text or binary form.
   */
  void handleSerializedMessage(const protocol::internal::SerializedMessage& serializedMessage);

  /// Returns a const reference to the owning App instance.
  const App& app() const
  {
    return m_app;
  }

  /// Returns a mutable reference to the owning App instance.
  App& app()
  {
    return m_app;
  }

  /// Returns the unique client identifier.
  const irsol::types::client_id_t& id() const
  {
    return m_id;
  }

  /// Returns a const reference to the client socket.
  const irsol::types::socket_t& socket() const
  {
    return m_socket;
  }

  /// Returns a mutable reference to the client socket.
  irsol::types::socket_t& socket()
  {
    return m_socket;
  }

  /// Returns a reference to the socket mutex used for synchronization.
  std::mutex& socketMutex()
  {
    return m_socketMutex;
  }

  /// Returns a const reference to client-specific session state.
  const irsol::server::internal::UserSessionData& userData() const
  {
    return m_sessionData;
  }

  /// Returns a mutable reference to client-specific session state.
  irsol::server::internal::UserSessionData& userData()
  {
    return m_sessionData;
  }

private:
  /**
   * @brief Processes accumulated raw data into complete protocol messages.
   *
   * Splits the incoming input buffer into individual protocol messages and dispatches each
   * one for parsing and handling.
   *
   * Invokes @ref ClientSession::processInRawMessage.
   *
   * @param messageBuffer Text buffer containing raw data received from the client's socket.
   */
  void processInMessageBuffer(std::string& messageBuffer);

  /**
   * @brief Parses and processes a complete incoming raw message.
   *
   * Invokes the registered message handler for the message type.
   *
   * @param rawMessage Complete raw protocol message string.
   */
  void processInRawMessage(const std::string& rawMessage);

  /**
   * @brief Sends a text message over the socket to the client.
   *
   * This is the way the server communicates to the client.
   *
   * @warning This function is not thread-safe. Caller must lock `m_socketMutex`.
   *
   * @param message The message string to send.
   */
  void send(const std::string& message);

  /**
   * @brief Sends binary data over the socket.
   *
   * @warning This function is not thread-safe. Caller must lock `m_socketMutex`.
   *
   * @param data Pointer to binary data buffer.
   * @param size Size of the data in bytes.
   */
  void send(const irsol::types::byte_t* const data, size_t size);

  /// Unique ID identifying this session (maps to client_id_t).
  irsol::types::client_id_t m_id;

  /// Socket used for communication with the client.
  irsol::types::socket_t m_socket;

  /// Mutex to synchronize socket access from multiple threads.
  std::mutex m_socketMutex{};

  /// Session-specific data (e.g., stream rate, frame subscriptions).
  irsol::server::internal::UserSessionData m_sessionData{};

  /// Reference to the central App instance for server-wide coordination.
  App& m_app;
};

}  // namespace server
}  // namespace irsol
