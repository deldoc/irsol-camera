# 06-client-server-interaction-image-commands {#client_server_interaction_image_commands}

@see examples/06-client-server-interaction-image-commands/server.cpp
@see examples/06-client-server-interaction-image-commands/viewer_client_gi.cpp
@see examples/06-client-server-interaction-image-commands/viewer_client_gis.cpp
@see irsol::server::frame_collector

This example demonstrates a basic client-server architecture for camera image acquisition and distribution using the `irsol` camera framework. It showcases how multiple clients can interact with a camera server by sending different image acquisition commands and receiving image data in response.

## Overview

- **Server**
  @see examples/06-client-server-interaction-image-commands/server.cpp
  The server executable runs the camera application server. It listens for incoming client connections and processes image acquisition commands, managing the camera hardware and distributing frames to clients as requested.

- **Viewer Client GI**
  @see examples/06-client-server-interaction-image-commands/viewer_client_gi.cpp
  This client executable repeatedly sends `gi` (get image) commands to the server. Each `gi` command requests a single image frame from the camera. The client receives and displays or processes each frame as it arrives.

- **Viewer Client GIS**
  @see examples/06-client-server-interaction-image-commands/viewer_client_gis.cpp
  This client executable repeatedly sends `gis` (get images) commands to the server. Each `gis` command requests a stream of images at a specified frame rate. The client receives a continuous stream of frames, which can be displayed after being all received.

## Purpose

This example illustrates how the `irsol` camera framework supports:

- **Command-based interaction:** Clients use specific commands (`gi` for single images, `gis` for streams) to control their interaction with the camera server.
- **Efficient frame distribution:** The server coordinates image acquisition and efficiently distributes frames to all connected clients, leveraging the internal scheduling and batching mechanisms of the framework.

## Context

This example is representative of real-world scenarios where a central camera server must serve multiple consumers with different image acquisition needs—such as monitoring stations, analysis tools, or visualization clients. It demonstrates the flexibility and scalability of the `irsol` camera framework's client-server model, as well as its ability to handle both on-demand and streaming image requests.
