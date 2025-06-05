#pragma once

#include "irsol/protocol/message.hpp"
#include "irsol/protocol/parsing.hpp"
#include "irsol/protocol/serialization.hpp"
#include "irsol/protocol/utils.hpp"

namespace irsol {
/**
 * @defgroup Protocol Protocol
 * @brief Comprehensive structured types, utilities, and mechanisms for irsol protocol message
 * exchange.
 *
 * The Protocol module encompasses the core abstractions and tools that define,
 * handle, and facilitate client-server communication in the irsol framework.
 *
 * This includes:
 *
 * - **Structured Message Types**: Strongly-typed C++ classes representing
 *   all protocol messages exchanged between client and server, categorized into:
 *   - **Incoming Messages** (@ref irsol::protocol::InMessage): commands,
 *     assignments, and inquiries originating from clients.
 *   - **Outgoing Messages** (@ref irsol::protocol::OutMessage): responses,
 *     status indications, binary data transmissions, and errors sent by servers.
 *
 * - **Tagged Variants for Polymorphism**: Use of `std::variant`-based
 *   discriminated unions to safely encapsulate and dispatch different message types.
 *
 * - **Parsing and Serialization**: Facilities to convert raw protocol data
 *   streams into validated, typed message instances, and vice versa, including
 *   serialization of primitive protocol values and complex message payloads.
 *
 * - **Protocol Traits and Introspection**: Compile-time traits and
 *   utilities for detecting valid message types, aiding generic programming,
 *   static dispatch, and template specialization.
 *
 * - **Protocol Framing and Special Bytes**: Constants and conventions
 *   (e.g., SOH, STX, ETX markers, message termination sequences) adhering
 *   to the Simple Communication Protocol to ensure reliable framing and parsing.
 *
 * - **Binary Data Support**: Definitions and serialization support for
 *   binary data payloads within messages, including images and color data.
 *
 * - **Robust Error Reporting**: Message types dedicated to error signaling
 *   with rich diagnostic information to facilitate debugging and recovery.
 *
 * - **Human-Readable String Representations**: Methods to produce
 *   textual representations of messages for logging, monitoring,
 *   and diagnostics.
 *
 * This module is designed with maintainability and extensibility in mind,
 * enabling the protocol to evolve without breaking existing communication contracts.
 */

/**
 * @namespace irsol::protocol
 * @brief Core protocol message types, serialization, parsing, and communication utilities for the
 * irsol framework.
 *
 * The `irsol::protocol` namespace encapsulates all components related to
 * the structured communication protocol between clients and servers
 * within the irsol ecosystem.
 *
 * Key responsibilities and features include:
 *
 * - **Message Representations**: Defines strongly typed C++ structures
 *   representing protocol messages, including incoming commands, outgoing
 *   responses, errors, and data payloads.
 *
 * - **Tagged Variants**: Groups related message types using `std::variant`
 *   or similar tagged unions to enable type-safe polymorphism for incoming
 *   and outgoing messages.
 *
 * - **Serialization and Deserialization**: Provides classes and functions
 *   to serialize protocol messages and primitive protocol values into
 *   wire-format strings or binary buffers, and to parse raw input into
 *   strongly typed message instances.
 *
 * - **Protocol Traits and Type Introspection**: Supplies compile-time
 *   traits to check if a type is part of a message variant, facilitating
 *   generic programming and template dispatch.
 *
 * - **Parsing Utilities**: Implements parsers that consume raw input data,
 *   producing parse trees or structured message objects, supporting
 *   validation and error reporting.
 *
 * - **Communication Primitives**: Defines low-level constructs such as
 *   special byte constants (e.g., SOH, STX, ETX), message delimiters,
 *   and framing conventions consistent with the Simple Communication Protocol
 *   employed by irsol.
 *
 * - **Logging and Debugging Support**: Converts protocol messages and
 *   events into human-readable string representations for logging,
 *   diagnostics, and tracing.
 *
 * - **Extensibility and Maintenance**: Designed with clear separation
 *   of concerns and compile-time safety to allow future protocol message
 *   extensions, new message types, or serialization formats without
 *   compromising existing codebases.
 *
 * Together, these features enable robust, efficient, and maintainable
 * communication handling in client-server interactions, ensuring
 * interoperability and clear semantics across the irsol system.
 *
 * @note Most message and utility types are contained in sub-namespaces
 *   such as `internal` for implementation details, and `traits` for
 *   type traits.
 *
 * @ingroup Protocol
 */

namespace protocol {}
}