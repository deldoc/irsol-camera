# 03-message-protocols {#message_protocols}
@see examples/03-message-protocols/main.cpp


Welcome to the `irsol::protocol` subsystem! This guide provides an overview and practical introduction to the core capabilities and design of the protocol communication layer used in the irsol framework.


## What is `irsol::protocol`?

The `irsol::protocol` namespace defines the **structured communication protocol** used for message exchange between clients and servers within irsol. It offers:

- **Strongly-typed message definitions** representing commands, responses, errors, and data exchanges.
- **Parsing and serialization utilities** to convert between raw protocol data streams and in-memory message objects.
- **Type-safe dispatch and introspection** via tagged variants and compile-time traits.
- **Support for both textual and binary messages**, including image data.
- **Error handling and diagnostic messages** for robust communication.

The protocol is designed for clarity, safety, and extensibility, making it easy to integrate new message types and evolve the protocol without breaking existing clients or servers.


## Core Concepts

### 1. Message Direction

Messages in the protocol fall into two main categories based on who sends them:

- **Incoming Messages** — @ref irsol::protocol::InMessage sent by clients to request actions or information, such as:
  - **Commands** — to trigger operations (@ref irsol::protocol::Command).
  - **Assignments** — to set values (@ref irsol::protocol::Assignment).
  - **Inquiries** — to query current values (@ref irsol::protocol::Inquiry).

- **Outgoing Messages** — @ref irsol::protocol::OutMessage sent by servers as responses or notifications, such as:
  - **Success** — confirmation of successful operations (@ref irsol::protocol::Success).
  - **Error** — reports of failures or invalid requests (@ref irsol::protocol::Error).
  - **Binary Data** — payloads such as images or raw data buffers (@ref irsol::protocol::internal::BinaryData).

### 2. Message Variants and Type Safety

Instead of handling raw strings or loosely structured data, messages are represented as **C++ variant types** containing strongly-typed classes. This approach ensures:

- Compile-time checks on valid message types.
- Safe, explicit parsing and serialization.
- Easy dynamic dispatch and message handling.

### 3. Serialization and Parsing

`irsol::protocol` provides utilities to:

- Convert messages to and from their **serialized string or binary form**.
- Handle **special protocol markers** (e.g., `SOH`, `STX`, `ETX` bytes).
- Support primitive protocol values (`int`, `double`, `string`) with proper formatting @see irsol::types::protocol_value_t.


## Diving Deeper

### Message Classes

Each message type is represented by a dedicated class with:

- Fields for each component of the protocol message.
- Validation rules to ensure correctness.
- Methods for converting to/from strings or binary.

For example:

- `Assignment` messages contain an identifier and a value to set.
- `Success` messages confirm the completion of an operation.
- `BinaryDataBuffer` messages encapsulate raw data payloads.

### Parsing Incoming Data

Although not covered in detail here, the protocol provides a @ref irsol::protocol::Parser to:

- Convert raw incoming byte streams into @ref irsol::protocol::InMessage instances.
- Validate message structure and contents.
- Extract binary payloads safely.

### Serialization with `Serializer`

The @ref irsol::protocol::Serializer class provides static methods to:

- Serialize any @ref irsol::protocol::OutMessage variant into a @ref irsol::protocol::internal::SerializedMessage.
- Serialize primitive protocol values with proper escaping and formatting.
- Handle message framing using protocol-specific byte markers and line terminators.

Serialization consumes the message object, ensuring efficient handling of large or binary payloads.


### Error Handling

Errors in the protocol are communicated via @ref irsol::protocol::Error messages containing:

- An error code.
- A human-readable message.
- Optional diagnostic data.

This mechanism ensures that clients receive clear feedback on invalid requests or unexpected conditions.
