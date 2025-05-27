#pragma once

#include "irsol/types.hpp"

#include <memory>

namespace irsol {
namespace server {
// Forward declaration
class App;
namespace internal {
class ClientSession;
}
namespace handlers {
struct Context
{
  App&                                                      app;
  std::shared_ptr<::irsol::server::internal::ClientSession> getSession(
    const ::irsol::types::client_id_t& clientId);
};
}
}
}