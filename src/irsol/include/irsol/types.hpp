#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_connector.h>
#include <string>
#include <variant>

namespace irsol {
namespace types {

// Server-related types
using port_t      = uint16_t;
using client_id_t = std::string;

using acceptor_t          = sockpp::tcp_acceptor;
using connector_t         = sockpp::tcp_connector;
using inet_address_t      = sockpp::inet_address;
using socket_t            = sockpp::tcp_socket;
using connection_result_t = sockpp::result<>;

// Time-related types
using clock_t     = std::chrono::steady_clock;
using timepoint_t = clock_t::time_point;
using duration_t  = clock_t::duration;

// Protocol-related types
using protocol_value_t = std::variant<int, double, std::string>;

// Serialization-related types
using byte_t = std::byte;
}
}