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

namespace internal {

// Forward declarations
class ClientSession;
class FrameCollector;

}  // namespace internal

/**
 * @brief Main server application that manages client connections and camera streaming.
 *
 * App listens for incoming TCP connections on a specified port, creates
 * ClientSession instances for each connected client, and orchestrates
 * broadcasting of messages. It owns the camera interface
 * and the FrameCollector, ensuring that captured frames are distributed to
 * all active clients according to their needs.
 */
class App
{

  using client_map_t =
    std::unordered_map<irsol::types::client_id_t, std::shared_ptr<internal::ClientSession>>;

public:
  /**
   * @brief Constructs the server application bound to a given port.
   *
   * @param port TCP port to listen on for client connections.
   */
  explicit App(irsol::types::port_t port);

  /**
   * @brief Starts the server: begins listening and accepting clients.
   *
   * Spawns the accept loop thread. Returns true on successful startup.
   *
   * @return true if the server started correctly, false otherwise.
   */
  bool start();

  /**
   * @brief Stops the server: shuts down accepting new clients and all active sessions.
   *
   * Signals the accept loop to exit, stops the FrameCollector, closes all
   * client sessions, and joins threads.
   */
  void stop();

  /**
   * @brief Retrieves a client session by its unique identifier.
   *
   * @param clientId Unique identifier of the client.
   * @return A shared pointer to the client session, or nullptr if not found.
   */
  std::shared_ptr<internal::ClientSession> getClientSession(
    const irsol::types::client_id_t& clientId);

  camera::Interface& camera()
  {
    return *m_cameraInterface;
  };
  frame_collector::FrameCollector& frameCollector()
  {
    return *m_frameCollector;
  };
  const handlers::MessageHandler& messageHandler() const
  {
    return *m_messageHandler;
  };

private:
  /// TCP port on which the server listens for incoming connections.
  const irsol::types::port_t m_port;

  /// Acceptor for new client connections.
  irsol::server::internal::ClientSessionAcceptor m_acceptor;

  /// Thread that runs the acceptor for new clients.
  std::thread m_acceptThread;

  /// Protects concurrent access to the client sessions map.
  std::mutex m_clientsMutex;

  /// Maps client IDs to their active ClientSession instances.
  client_map_t m_clients;

  /// Owned interface to the camera hardware.
  std::unique_ptr<camera::Interface> m_cameraInterface;

  /// Owned collector that captures frames and dispatches them to clients.
  std::unique_ptr<frame_collector::FrameCollector> m_frameCollector;

  /// Message handler for incoming messages.
  std::unique_ptr<handlers::MessageHandler> m_messageHandler;

  /**
   * @brief Registers a new client session.
   *
   * Inserts the session into m_clients and starts its run() method in a new thread..
   *
   * @param clientId Unique ID for the new client.
   * @param sock  TCP socket for the new client.
   * @note This function is called from the accept loop thread.
   */
  void addClient(const irsol::types::client_id_t& clientId, irsol::types::socket_t&& sock);

  /**
   * @brief Unregisters a disconnected client.
   *
   * Removes the session from m_clients, notifies FrameCollector to stop
   * streaming to that client, and cleans up resources.
   *
   * @param clientId ID of the client to remove.
   * @note: This function is called as soon as the client connection is closed (e.g. when client
   * disconnects).
   */
  void removeClient(const irsol::types::client_id_t& clientId);

  /**
   * @brief Registers message handlers for the server.
   *
   * Sets up handlers for assignment, inquiry, and command messages.
   */
  void registerMessageHandlers();

  /**
   * @brief Template function to register a message handler.
   *
   * @param identifier Identifier for the message type
   * @param args Arguments for the handler constructor.
   *
   * @note This function is templated to handle different message types and handlers.
   * @example
   * ```cpp
   * registerMessageHandler<protocol::Inquiry, handlers::InquiryFRHandler>("fr", ctx);
   * registerMessageHandler<protocol::Command, handlers::CommandFRHandler>("fr", ctx,
   * anotherParamForCommandFRHandler);
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
   * @brief Template function to register a lambda message handler.
   *
   * @param identifier Identifier for the message type
   * @param ctx Context for the lambda handler.
   * @param lambda Lambda function for the handler.
   *
   * @note This function is templated to handle different message types and lambda handlers.
   * @example
   * ```cpp
   * registerLambdaHandler<protocol::Command>(
   *  "lambda_command",
   *  ctx,
   *   [](
   *     handlers::Context&                  ctx,
   *     const ::irsol::types::client_id_t& clientId,
   *     protocol::Command&&                 cmd) -> std::vector<protocol::OutMessage> {
   *        // Handle the command
   *        return {...};
   *  }
   * );
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