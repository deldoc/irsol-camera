#pragma once

#include "sockpp/tcp_acceptor.h"
#include "sockpp/tcp_connector.h"

#include <cstdint>
#include <string>

namespace irsol {
namespace types {

// Server-related types
using port_t      = uint16_t;
using client_id_t = std::string;

using acceptor_t     = sockpp::tcp_acceptor;
using connector_t    = sockpp::tcp_connector;
using inet_address_t = sockpp::inet_address;
using socket_t       = sockpp::tcp_socket;

}
}