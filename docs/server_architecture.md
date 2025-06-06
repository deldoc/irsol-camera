# Server Architecture {#server_architecture}

This document provides a thorough overview of the architecture of a running `irsol` server. It is intended for developers and users who are new to the project and want to understand how the application is structured, how its main components interact, and how client connections are managed.


## Overview

The `irsol` server is designed to manage multiple client connections, coordinate access to a physical camera device, and distribute image frames to clients according to their requests. The architecture is modular, with clear separation of concerns between connection management, camera access, and message handling.

At the heart of the system is the @ref irsol::server::App class, which acts as the central coordinator. Each client that connects to the server is managed by its own @ref irsol::server::ClientSession, running in a dedicated thread. The @ref irsol::server::frame_collector::FrameCollector is responsible for acquiring images from the camera and distributing them to clients as needed.


## Main Components

### App
@see irsol::server::App
- **Role:** The central application object, responsible for orchestrating all server activities.
- **Responsibilities:**
  - Starts and stops the server.
  - Owns the main camera interface and the frame collector.
  - Maintains a registry of all active client sessions.
  - Provides access to shared resources for message handlers.
  - Broadcasts messages to all clients if needed.

### Acceptor
@see irsol::server::internal::ClientSessionAcceptor
- **Role:** Listens for incoming TCP connections from clients.
- **Responsibilities:**
  - Runs in its own thread.
  - Accepts new client connections.
  - For each new connection, creates a new `ClientSession` and hands off the socket.

### ClientSession
@see irsol::server::ClientSession
- **Role:** Represents a single connected client.
- **Responsibilities:**
  - Runs in its own thread, handling all communication with the client.
  - Reads and parses incoming messages.
  - Sends responses and image data back to the client.
  - Maintains private, per-client state (e.g., streaming preferences, session data).
  - Interacts with the central @ref irsol::server::App and shared resources via message handlers.

- **User Data:** Each `ClientSession` has its own `UserSessionData` structure, which holds all private state for that client, such as streaming parameters and thread control flags.

### FrameCollector
@see irsol::server::frame_collector::FrameCollector
- **Role:** Manages access to the physical camera and coordinates frame distribution.
- **Responsibilities:**
  - Runs a background thread to acquire images from the camera.
  - Maintains a schedule of which @ref irsol::server::ClientSession need frames and at what rates.
  - Pushes frames into per-client queues as requested.
  - Ensures thread-safe access to the camera hardware.

### Camera Interface
@see irsol::camera::Interface
- **Role:** Abstracts the physical camera device (e.g., Baumer camera).
- **Responsibilities:**
  - Provides methods to capture images, set parameters, and query camera status.
  - Used exclusively by the @ref irsol::server::frame_collector::FrameCollector to ensure safe, serialized access.

### Message Handlers
@see irsol::server::handlers
@see irsol::server::handlers::MessageHandler
- **Role:** Encapsulate the logic for processing protocol messages from clients.
- **Responsibilities:**
  - Each handler is responsible for a specific type of message (e.g., frame requests, parameter changes).
  - Handlers have access to the central @ref irsol::server::App and can interact with shared resources.
  - Enable modular and extensible message processing.


## Lifecycle of a Client Connection

1. **Startup:** The `App` starts the `Acceptor` on a specified TCP port.
2. **Connection:** When a client connects, the `Acceptor` creates a new `ClientSession` and assigns it a unique ID.
3. **Session Thread:** The `ClientSession` runs in its own thread, handling all communication with the client.
4. **Message Processing:** Incoming messages are parsed and dispatched to the appropriate message handler. The message send by one client are parsed and processed by the `ClientSession` associated with that client.
5. **Frame Requests:** If a client requests image frames, `ClientSession` registers itself in the `FrameCollector`, which in turn schedules and delivers frames to the client via a thread-safe queue.
6. **Disconnection:** When a client disconnects, its `ClientSession` is cleaned up, and any resources (such as frame queues) are released.


## Threading Model

- **App:** Main thread, coordinates startup and shutdown.
- **Acceptor:** Runs in a dedicated thread, accepting new connections. The `Acceptor` thread is owned by the `App`.
- **ClientSession:** Each client session runs in its own thread, ensuring that slow or blocked clients do not affect others. The `ClientSession` threads are detached on creation, and are automatically joined when the client disconnects from the server.
- **FrameCollector:** Runs a background thread to acquire and distribute frames.


## Shared Resources and Synchronization

- The `App` object and its associated resources (camera, frame collector) are shared among all client sessions.
- Access to shared data structures (such as the client registry and frame schedules) is protected by mutexes.
- Each `ClientSession` has its own private user data, ensuring that per-client state is isolated and thread-safe.


## Summary Diagram

Below is a simplified diagram illustrating the relationships between the main components:

```
+-------------------+
| App               |
| ----------------- |
| - CameraInterface |
| - FrameCollector  |
| - Acceptor        |
| - ClientSessions  |
+---+---+---+---+---+
        |   |   |
        |   |   +-------------------+
        |   +-------------------+   |
        +-------------------+   |   |
                            |   |   |
                        +---v---v---v---+
                        | ClientSession |
                        +---------------+
                        | UserSession   |
                        | (private data)|
                        +---------------+
```

- The `App` owns the camera interface, frame collector, and acceptor.
- Each `ClientSession` is created by the acceptor and runs in its own detached thread.
- The `FrameCollector` manages access to the camera and distributes frames to clients via a queue.
- Message handlers provide the logic for processing client requests and interact with the `App`.
