/**
 * @file irsol/types.hpp
 * @brief Core type definitions for networking, time handling, and protocol values used throughout
 * the irsol library.
 *
 * This header defines fundamental type aliases related to server networking,
 * time measurements, protocol data handling, and serialization, providing
 * consistent and readable types across the codebase.
 */

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_connector.h>
#include <string>
#include <variant>

namespace irsol {

/**
 * @namespace irsol::types
 * @brief Contains fundamental type aliases used throughout the irsol library.
 *
 * The `types` namespace centralizes definitions for:
 * - Server networking primitives (e.g., sockets, acceptors, client identifiers).
 * - Time-related types for steady clock timing and durations.
 * - Protocol-related variant types representing protocol values.
 * - Serialization byte type.
 *
 * This organization promotes consistency and code clarity.
 */
namespace types {

/**
 * @name Server-related Types
 * @brief Type aliases for server networking components.
 * @{
 */

/**
 * @typedef port_t
 * @brief Represents a network port number.
 * Typically used to specify TCP or UDP ports.
 */
using port_t = uint16_t;

/**
 * @typedef client_id_t
 * @brief Represents a unique client identifier.
 * Typically used to identify connected clients by string IDs.
 */
using client_id_t = std::string;

/**
 * @typedef acceptor_t
 * @brief Alias for the TCP server socket acceptor type.
 *
 * Uses `sockpp::tcp_acceptor` to listen for incoming TCP connection requests.
 */
using acceptor_t = sockpp::tcp_acceptor;

/**
 * @typedef connector_t
 * @brief Alias for the TCP client connector type.
 *
 * Uses `sockpp::tcp_connector` to initiate outbound TCP connections.
 */
using connector_t = sockpp::tcp_connector;

/**
 * @typedef inet_address_t
 * @brief Alias for an IPv4/IPv6 internet address.
 *
 * Uses `sockpp::inet_address` for IP address representation.
 */
using inet_address_t = sockpp::inet_address;

/**
 * @typedef socket_t
 * @brief Alias for a TCP socket.
 *
 * Uses `sockpp::tcp_socket` for bidirectional TCP communication.
 */
using socket_t = sockpp::tcp_socket;

/**
 * @typedef connection_result_t
 * @brief Result type representing success or failure of a socket operation.
 *
 * Uses `sockpp::result<>` to encapsulate operation outcomes.
 */
using connection_result_t = sockpp::result<>;

/** @} */  // end Server-related Types

/**
 * @name Time-related Types
 * @brief Type aliases related to steady clock time points and durations.
 * @{
 */

/**
 * @typedef clock_t
 * @brief Alias for the steady clock type used throughout the library.
 *
 * Steady clock provides monotonic time measurement to avoid issues
 * with system clock changes.
 */
using clock_t = std::chrono::steady_clock;

/**
 * @typedef timepoint_t
 * @brief Alias for a point in time as defined by `clock_t`.
 *
 * Represents a timestamp or moment in steady clock time.
 */
using timepoint_t = clock_t::time_point;

/**
 * @typedef duration_t
 * @brief Alias for a duration of time as defined by `clock_t`.
 *
 * Represents an elapsed time interval.
 */
using duration_t = clock_t::duration;

/** @} */  // end Time-related Types

/**
 * @name Protocol-related Types
 * @brief Variant types representing values in the communication protocol.
 * @{
 */

/**
 * @typedef protocol_value_t
 * @brief Variant type representing protocol values that can be one of several types.
 *
 * Used to store protocol fields flexibly, supporting:
 * - `int` for integer values,
 * - `double` for floating-point values,
 * - `std::string` for textual values.
 *
 * @see irsol::protocol::Assignment
 * @see irsol::protocol::Parser::parseValue
 */
using protocol_value_t = std::variant<int, double, std::string>;

/** @} */  // end Protocol-related Types

/**
 * @name Serialization-related Types
 * @{
 */

/**
 * @typedef byte_t
 * @brief Alias for a single byte used in serialization or binary data handling.
 *
 * Uses `std::byte` from the C++ standard library for type-safe byte representation.
 */
using byte_t = std::byte;

/** @} */  // end Serialization-related Types

}  // namespace types
}  // namespace irsol
