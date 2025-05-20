#pragma once

#include "sockpp/tcp_socket.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>

namespace irsol {

// Forward declaration
class ServerApp;

namespace internal {

/**
 * @brief Parameters controlling how and when frames are delivered to a client.
 *
 * lastFrameSent: timestamp of the last frame successfully sent to the client.
 * frameRate: desired frame rate (in frames per second) for this client.
 */
struct FrameListeningParams {
  std::chrono::time_point<std::chrono::steady_clock> lastFrameSent{
      std::chrono::steady_clock::now()};
  double frameRate;
};

/**
 * @brief Encapsulates all per-client data for managing a user session.
 *
 * A UserSessionData object holds the networking socket for communication,
 * synchronization primitives protecting concurrent access to the socket,
 * and frame delivery parameters that control how images are streamed to the client.
 */
struct UserSessionData {

  /**
   * @brief Constructs a new UserSessionData with a given TCP socket.
   *
   * @param sock The TCP socket representing the client's connection.
   */
  UserSessionData(sockpp::tcp_socket &&sock);

  /// Controls the parameters for streaming image frames to the client.
  FrameListeningParams frameListeningParams{};

  /// The TCP socket used to communicate with the client.
  sockpp::tcp_socket sock;

  /// Mutex to protect access to the socket and associated buffers.
  std::mutex mutex{};
};

/**
 * @brief Represents a single connected client session in the server.
 *
 * A ClientSession instance encapsulates all state and operations related to one
 * client's connection: reading incoming messages, sending data, and participating
 * in frame streaming. It holds the client's unique identifier, the socket and
 * session-specific data in UserSessionData, and a reference to the central
 * ServerApp for routing messages and integrating with server-wide logic.
 *
 * std::enable_shared_from_this is used to allow safe creation of std::shared_ptr
 * instances referring to *this. This enables passing shared ownership of the
 * ClientSession into asynchronous callbacks and threads without risking the
 * object being destroyed while in use.
 */
class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
  /**
   * @brief Construct a new ClientSession.
   *
   * @param id     A unique identifier for this client (e.g., UUID or socket-based ID).
   * @param sock   The TCP socket that is already connected to the client.
   * @param app    Reference to the ServerApp for which this session is associated.
   */
  ClientSession(const std::string &id, sockpp::tcp_socket &&sock, ServerApp &app);

  /**
   * @brief Entry point to start processing this client's lifecycle.
   *
   * Typically this will read from the socket, handle messages, and respond
   * until the client disconnects.
   */
  void run();

  /**
   * @brief Send a text message to the client.
   *
   * @note This method is not thead safe. It's the user's responsibility to ensure the session mutex
   * is held when calling this method.
   *
   * @param message String data to transmit.
   */
  void send(const std::string &message);

  /**
   * @brief Send raw binary data to the client.
   *
   * @note This method is not thead safe. It's the user's responsibility to ensure the session mutex
   * is held when calling this method.
   *
   * @param data Pointer to the buffer.
   * @param size Number of bytes to send.
   */
  void send(void *data, size_t size);

  /// Accessor for the immutable ServerApp reference.
  const ServerApp &app() const { return m_app; }
  /// Accessor for the mutable ServerApp reference.
  ServerApp &app() { return m_app; }

  /// Get the unique client identifier.
  const std::string &id() const { return m_id; }

  /// Immutable access to this session's UserSessionData.
  const UserSessionData &sessionData() const { return m_sessionData; }
  /// Mutable access to this session's UserSessionData.
  UserSessionData &sessionData() { return m_sessionData; }

private:
  /**
   * @brief Process accumulated text commands in the message buffer.
   *
   * Splits the buffer into discrete messages and passes them to processRawMessage().
   */
  void processMessageBuffer(std::string &messageBuffer);

  /**
   * @brief Handle a single complete raw message from the client.
   *
   * Parses the message and triggers appropriate server actions.
   *
   * @param rawMessage The protocol message payload.
   */
  void processRawMessage(const std::string &rawMessage);

  /// Unique identifier for this client session.
  std::string m_id;

  /// Holds the socket, mutex, and frame streaming parameters.
  UserSessionData m_sessionData;

  /// Reference back to the owning server application for callbacks and state.
  ServerApp &m_app;
};
} // namespace internal
} // namespace irsol