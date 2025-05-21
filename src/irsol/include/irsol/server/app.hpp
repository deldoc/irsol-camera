#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/client_session.hpp"
#include "irsol/server/collector.hpp"
#include <atomic>
#include <mutex>
#include <sockpp/tcp_acceptor.h>
#include <thread>
#include <unordered_map>

namespace irsol {

namespace server {

namespace internal {

// Forward declarations
class ClientSession;
class FrameCollector;

} // namespace internal

/**
 * @brief Main server application that manages client connections and camera streaming.
 *
 * App listens for incoming TCP connections on a specified port, creates
 * ClientSession instances for each connected client, and orchestrates
 * broadcasting of messages. It owns the camera interface
 * and the FrameCollector, ensuring that captured frames are distributed to
 * all active clients according to their needs.
 */
class App {
public:
  /**
   * @brief Constructs the server application bound to a given port.
   *
   * @param port TCP port to listen on for client connections.
   */
  explicit App(uint32_t port);

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
   * @brief Broadcasts a textual message to all connected clients.
   *
   * Thread-safe: locks the client map before sending. Useful for server-wide
   * notifications, errors, or control messages.
   *
   * @param message The serialized message to send.
   */
  void broadcast(const std::string &message);

  camera::Interface &camera();
  internal::FrameCollector &frameCollector();

private:
  /// TCP port on which the server listens for incoming connections.
  const uint32_t m_port;

  /// Atomic flag controlling the server's running state.
  std::atomic<bool> m_running;

  /// Thread that runs the accept loop for new clients.
  std::thread m_acceptThread;

  /// Acceptor socket bound to m_port.
  sockpp::tcp_acceptor m_acceptor;

  /// Protects concurrent access to the client sessions map.
  std::mutex m_clientsMutex;

  /// Maps client IDs to their active ClientSession instances.
  std::unordered_map<std::string, std::shared_ptr<internal::ClientSession>> m_clients;

  /// Owned interface to the camera hardware.
  std::unique_ptr<camera::Interface> m_cameraInterface;

  /// Owned collector that captures frames and dispatches them to clients.
  std::unique_ptr<internal::FrameCollector> m_frameCollector;

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
  void addClient(const std::string &clientId, std::shared_ptr<internal::ClientSession> session);

  /**
   * @brief Unregisters a disconnected client.
   *
   * Removes the session from m_clients, notifies FrameCollector to stop
   * streaming to that client, and cleans up resources.
   *
   * @param clientId ID of the client to remove.
   */
  void removeClient(const std::string &clientId);
};
} // namespace server
} // namespace irsol