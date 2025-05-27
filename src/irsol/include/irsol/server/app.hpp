#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/client.hpp"
#include "irsol/server/collector.hpp"
#include "irsol/server/message_handler.hpp"
#include "irsol/server/types.hpp"

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

  using client_map_t = std::unordered_map<client_id_t, std::shared_ptr<internal::ClientSession>>;

public:
  /**
   * @brief Constructs the server application bound to a given port.
   *
   * @param port TCP port to listen on for client connections.
   */
  explicit App(port_t port);

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
  std::shared_ptr<internal::ClientSession> getClientSession(const client_id_t& clientId);

  camera::Interface& camera()
  {
    return *m_cameraInterface;
  };
  internal::FrameCollector& frameCollector()
  {
    return *m_frameCollector;
  };
  const handlers::MessageHandler& messageHandler() const
  {
    return *m_messageHandler;
  };

private:
  /// TCP port on which the server listens for incoming connections.
  const port_t m_port;

  /// Atomic flag controlling the server's running state.
  std::atomic<bool> m_running;

  /// Thread that runs the accept loop for new clients.
  std::thread m_acceptThread;

  /// Acceptor socket bound to m_port.
  acceptor_t m_acceptor;

  /// Protects concurrent access to the client sessions map.
  std::mutex m_clientsMutex;

  /// Maps client IDs to their active ClientSession instances.
  client_map_t m_clients;

  /// Owned interface to the camera hardware.
  std::unique_ptr<camera::Interface> m_cameraInterface;

  /// Owned collector that captures frames and dispatches them to clients.
  std::unique_ptr<internal::FrameCollector> m_frameCollector;

  /// Message handler for incoming messages.
  std::unique_ptr<handlers::MessageHandler> m_messageHandler;

  /**
   * @brief Loop that accepts new TCP client connections.
   *
   * Runs in m_acceptThread. On each new connection, generates a unique client
   * ID, creates a ClientSession, and invokes addClient().
   */
  void acceptLoop();

  /**
   * @brief Registers a new client session.
   *
   * Inserts the session into m_clients and starts its run() method in a new thread..
   *
   * @param clientId Unique ID for the new client.
   * @param session  Shared pointer to the created ClientSession.
   */
  void addClient(const client_id_t& clientId, std::shared_ptr<internal::ClientSession> session);

  /**
   * @brief Unregisters a disconnected client.
   *
   * Removes the session from m_clients, notifies FrameCollector to stop
   * streaming to that client, and cleans up resources.
   *
   * @param clientId ID of the client to remove.
   */
  void removeClient(const client_id_t& clientId);

  /**
   * @brief Registers message handlers for the server.
   *
   * Sets up handlers for assignment, inquiry, and command messages.
   */
  void registerMessageHandlers();
};
}  // namespace server
}  // namespace irsol