/**
 * @file irsol/server/app.hpp
 * @brief Main server application managing client connections and camera streaming.
 * @ingroup Server
 *
 * Defines the @ref irsol::server::App class, which orchestrates the server-side infrastructure
 * including:
 * - Accepting client connections
 * - Managing client sessions
 * - Distributing captured frames from the camera
 * - Handling incoming protocol messages
 *
 * This class owns all components necessary to run a live server application.
 */

#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/acceptor.hpp"
#include "irsol/server/client.hpp"
#include "irsol/server/handlers/factory.hpp"
#include "irsol/server/image_collector.hpp"
#include "irsol/server/message_handler.hpp"
#include "irsol/types.hpp"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace irsol {
namespace server {

// Forward declaration
class ClientSession;

/**
 * @brief Main server application that manages client connections and camera streaming.
 * @ingroup Server
 *
 * The App class starts the TCP server, listens for new connections,
 * instantiates a new @ref irsol::server::ClientSession per client, and coordinates the camera
 * interface, frame collection, and message dispatching.
 */
class App
{
  using client_map_t =
    std::unordered_map<irsol::types::client_id_t, std::shared_ptr<ClientSession>>;

public:
  /**
   * @brief Constructs the App.
   * @param port The TCP port on which the server will listen for client connections.
   */
  explicit App(irsol::types::port_t port);

  /**
   * @brief Starts the server.
   * @return True if the server starts successfully, false otherwise.
   *
   * This starts the acceptor thread, initializes camera and message handlers,
   * and prepares the system for client interaction.
   */
  bool start();

  /**
   * @brief Stops the server.
   *
   * Gracefully shuts down client sessions, stops the frame collector,
   * and joins the acceptor thread.
   */
  void stop();

  /**
   * @brief Retrieves an active client session.
   * @param clientId The unique ID of the client.
   * @return Shared pointer to the client session, or nullptr if not found.
   */
  std::shared_ptr<ClientSession> getClientSession(const irsol::types::client_id_t& clientId);

  /**
   * @brief Broadcasts a message to all connected clients.
   * @param message The message to send.
   * @param excludeClient Optional client to exclude (e.g., the sender).
   *
   * This method is thread-safe and will skip the excluded client if specified.
   */
  void broadcastMessage(
    protocol::OutMessage&&                          message,
    const std::optional<irsol::types::client_id_t>& excludeClient = std::nullopt);

  /**
   * @brief Accessor for the camera interface.
   * @return Reference to the owned camera interface.
   */
  camera::Interface& camera()
  {
    return *m_cameraInterface;
  };

  /**
   * @brief Accessor for the frame collector.
   * @return Reference to the owned frame collector.
   */
  frame_collector::FrameCollector& frameCollector()
  {
    return *m_frameCollector;
  }

  /**
   * @brief Accessor for the message handler.
   * @return Const reference to the owned message handler.
   */
  const handlers::MessageHandler& messageHandler() const
  {
    return *m_messageHandler;
  }

private:
  /// TCP port on which the server listens.
  const irsol::types::port_t m_port;

  /// Acceptor that handles incoming client connections.
  irsol::server::internal::ClientSessionAcceptor m_acceptor;

  /// Thread running the connection acceptor loop.
  std::thread m_acceptThread;

  /// Mutex for protecting access to m_clients.
  std::mutex m_clientsMutex;

  /// Map of connected clients.
  client_map_t m_clients;

  /// Interface to the camera.
  std::unique_ptr<camera::Interface> m_cameraInterface;

  /// Frame collector for capturing and broadcasting camera frames.
  std::unique_ptr<frame_collector::FrameCollector> m_frameCollector;

  /// Central handler for processing protocol messages.
  std::unique_ptr<handlers::MessageHandler> m_messageHandler;

  /**
   * @brief Adds a new client session.
   * @param clientId Unique ID for the client.
   * @param sock TCP socket for communication with the client.
   *
   * @note The new session is started on a separate thread.
   */
  void addClient(const irsol::types::client_id_t& clientId, irsol::types::socket_t&& sock);

  /**
   * @brief Removes a client session.
   * @param clientId ID of the client to disconnect and clean up.
   *
   * This is typically called automatically when the client disconnects or times out.
   */
  void removeClient(const irsol::types::client_id_t& clientId);

  /**
   * @brief Registers standard message handlers.
   *
   * Sets up the message handler registry with known message-handler mappings.
   */
  void registerMessageHandlers();

  /**
   * @brief Registers a message handler by type.
   * @tparam InMessageT Incoming message type.
   * @tparam HandlerT Handler class for the message.
   * @tparam Args Constructor arguments for the handler.
   * @param identifier Message handler identifier string.
   * @param args Arguments to construct the handler.
   *
   * Example:
   * ```cpp
   * registerMessageHandler<protocol::Command, handlers::CommandFRHandler>("fr", ctx);
   * ```
   */
  template<typename InMessageT, typename HandlerT, typename... Args>
  void registerMessageHandler(const std::string& identifier, Args&&... args)
  {
    auto  handler        = handlers::makeHandler<HandlerT>(std::forward<Args>(args)...);
    auto& messageHandler = *m_messageHandler;

    if(!messageHandler.template registerHandler<InMessageT>(identifier, handler)) {
      IRSOL_LOG_FATAL("Failed to register handler for identifier {}", identifier);
      throw std::runtime_error("Failed to register handler for identifier '" + identifier + "'");
    }
  }

  /**
   * @brief Registers a lambda-based message handler.
   * @tparam InMessageT Incoming message type.
   * @tparam LambdaT Type of the lambda handler.
   * @param identifier Handler identifier.
   * @param ctx Handler context.
   * @param lambda Lambda function to invoke for the message.
   *
   * Example:
   * ```cpp
   * registerLambdaHandler<protocol::Command>(
   *   "custom_cmd",
   *   ctx,
   *   [](handlers::Context& ctx, const irsol::types::client_id_t& clientId, protocol::Command&&
   * cmd) { return std::vector<protocol::OutMessage>{...};
   *   }
   * );
   * ```
   */
  template<typename InMessageT, typename LambdaT>
  void
  registerLambdaHandler(const std::string& identifier, handlers::Context& ctx, LambdaT&& lambda)
  {
    auto  handler = handlers::makeLambdaHandler<InMessageT>(ctx, std::forward<LambdaT>(lambda));
    auto& messageHandler = *m_messageHandler;

    if(!messageHandler.template registerHandler<InMessageT>(identifier, handler)) {
      IRSOL_LOG_FATAL("Failed to register lambda handler for identifier {}", identifier);
      throw std::runtime_error(
        "Failed to register lambda handler for identifier '" + identifier + "'");
    }
  }
};

}  // namespace server
}  // namespace irsol
