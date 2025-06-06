# 05-client-server-interaction {#client_server_interaction}

@see examples/05-client-server-interaction/main.cpp

This guide will introduce you to the basics of client-server communication in the `irsol` system, even if you have never worked with `irsol` or network programming before. We will cover:

- What sockets are and why they're important
- How the `irsol` server uses sockets to communicate
- How to build a simple C++ client to connect, send, and receive messages

## 1. What Are Sockets?

A **socket** is an endpoint for sending or receiving data across a computer network. Think of it as a virtual "plug" that allows two programs (often on different computers) to talk to each other.

- **Server socket:** Listens for incoming connections.
- **Client socket:** Connects to a server and exchanges data.

**Note**: communication through the socket is bidirectional and asynchronous. This is very different from other communication strategies like the "request-response" model.


```
[Server] <--- socket ---> [Client]
```

**Note**: a single server can have connections to multiple clients at the same time. Every connection is independently handled. In `irsol` every client is server by a dedicated thread that manages the communication with the client. @see server_architecture

## 2. How `irsol` Uses Sockets

The `irsol` server uses sockets to allow clients to connect and exchange protocol messages. Under the hood, `irsol` uses the [sockpp](https://github.com/fpagliughi/sockpp) C++ library to manage sockets in a cross-platform way.

- The **server app** creates a listening socket (TCP) on a specified port.
- Each **client** connects to this port using its own socket.
- Messages are exchanged using a defined protocol (see @ref irsol::protocol).

**`irsol` Server App Flow:**

1. Start server, bind to a port (e.g., 5555).
2. Wait for client connections.
3. For each client:
    - Accept connection.
    - Read messages, process, and send responses.


## 3. Example: Building a Simple C++ Client

Let's walk through a minimal C++ client that connects to an `irsol` server, sends a message, and receives a response.

### Prerequisites

- C++17 or later
- [sockpp](https://github.com/fpagliughi/sockpp) library installed

### Example Client Code

```cpp
#include <sockpp/tcp_connector.h>
#include <iostream>

int main() {
    // Connect to server at localhost:5555
    sockpp::tcp_connector conn({"127.0.0.1", 5555});
    if (!conn) {
        std::cerr << "Error connecting to server\n";
        return 1;
    }

    // Send a message (for example, a simple string)
    std::string msg = "Hello, `irsol` server!";
    if (conn.write(msg) != msg.size()) {
        std::cerr << "Error sending message\n";
        return 1;
    }

    // Receive a response
    char buf[256];
    ssize_t n = conn.read(buf, sizeof(buf));
    if (n > 0) {
        std::cout << "Received: " << std::string(buf, n) << std::endl;
    } else {
        std::cerr << "Error receiving response\n";
    }

    return 0;
}
```
**Note**: In a real `irsol` system, you would use the `irsol` protocol to format messages, not just plain strings. @see irsol::protocol

## 4. How the `irsol` Server Handles Clients
When you run the `irsol` server app, it:

* Listens for incoming TCP connections.
* For each client, creates a session and processes messages using registered handlers in a client-dedicate thread.
* Sends responses back to the client, either as an immediate action triggered from the incoming message (e.g. a @ref irsol::protocol::Inquiry), or asynchronously (e.g. processing the `gis` command).

```
+-------------------+      +-------------------+
| IRSOL Client 1    | ---> |                   |
+-------------------+      |                   |
                           |                   |
+-------------------+      |                   |
| IRSOL Client 2    | ---> |   IRSOL Server    |
+-------------------+      |                   |
                           |                   |
+-------------------+      |                   |
| IRSOL Client N    | ---> |                   |
+-------------------+      +-------------------+
```

## 5. Next Steps
@see examples/05-client-server-interaction/main.cpp

Explore the example server and client code in this folder.
Try running the server and connecting with multiple clients.
Experiment with sending different messages and handling responses.
For more details on the `irsol` protocol and message handlers, see the other examples and the API documentation.