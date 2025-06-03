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
 * @brief Represents a single connected client session in the server.
 *
 * A ClientSession instance encapsulates all state and operations related to one
 * client's connection: reading incoming messages, sending data, and participating
 * in frame streaming. It holds the client's unique identifier, the socket and
 * session-specific data in UserSessionData, and a reference to the central
 * App for routing messages and integrating with server-wide logic.
 *
 * std::enable_shared_from_this is used to allow safe creation of std::shared_ptr
 * instances referring to *this. This enables passing shared ownership of the
 * ClientSession into asynchronous callbacks and threads without risking the
 * object being destroyed while in use.
 */
class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
  /**
   * @brief Construct a new ClientSession.
   *
   * @param id     A unique identifier for this client (e.g., UUID or socket-based ID).
   * @param sock   The TCP socket that is already connected to the client.
   * @param app    Reference to the App for which this session is associated.
   */
  ClientSession(const irsol::types::client_id_t& id, irsol::types::socket_t&& sock, App& app);

  /**
   * @brief Entry point to start processing this client's lifecycle.
   *
   * Typically this will read from the socket, handle messages, and respond
   * until the client disconnects.
   */
  void run();

  /**
   * @brief Handle multiple message to send to the client.
   *
   * @note This method is not thread safe. It's the user's responsibility to ensure the session
   */
  void handleOutMessages(std::vector<protocol::OutMessage>&& messages);

  /**
   * @brief Handle a message to send to the client.
   *
   * @note This method is not thread safe. It's the user's responsibility to ensure the session
   */
  void handleOutMessage(protocol::OutMessage&& message);

  /**
   * @brief Handle an already serialized message and send it to the client.
   *
   * @note This method is not thread safe. It's the user's responsibility to ensure the session
   */
  void handleSerializedMessage(const protocol::internal::SerializedMessage& serializedMessage);

  /// Accessor for the immutable App reference.
  const App& app() const
  {
    return m_app;
  }
  /// Accessor for the mutable App reference.
  App& app()
  {
    return m_app;
  }

  /// Get the unique client identifier.
  const irsol::types::client_id_t& id() const
  {
    return m_id;
  }

  /// Immutable access to the client's socket.
  const irsol::types::socket_t& socket() const
  {
    return m_socket;
  }
  /// Mutable access to the client's socket.
  irsol::types::socket_t& socket()
  {
    return m_socket;
  }
  /// Get the socket mutex.
  std::mutex& socketMutex()
  {
    return m_socketMutex;
  }

  /// Immutable access to this session's UserSessionData.
  const irsol::server::internal::UserSessionData& userData() const
  {
    return m_sessionData;
  }
  /// Mutable access to this session's UserSessionData.
  irsol::server::internal::UserSessionData& userData()
  {
    return m_sessionData;
  }

private:
  /**
   * @brief Process accumulated text commands in the message buffer.
   *
   * Splits the buffer into discrete messages and passes them to processRawMessage().
   */
  void processMessageBuffer(std::string& messageBuffer);

  /**
   * @brief Handle a single complete raw message from the client.
   *
   * Parses the message and triggers appropriate server actions.
   *
   * @param rawMessage The protocol message payload.
   */
  void processRawMessage(const std::string& rawMessage);

  /**
   * @brief Send a text message to the client.
   *
   * @note This method is not thead safe. It's the user's responsibility to ensure the session mutex
   * is held when calling this method.
   *
   * @param message String data to transmit.
   */
  void send(const std::string& message);

  /**
   * @brief Send raw binary data to the client.
   *
   * @note This method is not thead safe. It's the user's responsibility to ensure the session mutex
   * is held when calling this method.
   *
   * @param data Pointer to the buffer.
   * @param size Number of bytes to send.
   */
  void send(const irsol::types::byte_t* const data, size_t size);

  /// Unique identifier for this client session.
  irsol::types::client_id_t m_id;

  /// Holds the socket and mutex for this session.
  irsol::types::socket_t m_socket;

  /// Mutex for managing access to the session's data.
  std::mutex m_socketMutex{};

  /// Holds the socket, mutex, and frame streaming parameters.
  irsol::server::internal::UserSessionData m_sessionData{};

  /// Reference back to the owning server application for callbacks and state.
  App& m_app;
};
}  // namespace server
}  // namespace irsol