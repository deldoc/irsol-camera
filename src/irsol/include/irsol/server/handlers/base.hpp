#pragma once

#include "irsol/protocol.hpp"
#include "irsol/server/handlers/context.hpp"
#include "irsol/traits.hpp"

#include <vector>

namespace irsol {
namespace server {

namespace handlers {
using in_message_t  = protocol::InMessage;
using out_message_t = protocol::OutMessage;

namespace internal {

template<
  typename T,
  std::enable_if_t<irsol::traits::is_type_in_variant<T, irsol::protocol::InMessage>::value, int> =
    0>
class HandlerBase
{
public:
  std::vector<out_message_t> operator()(const irsol::types::client_id_t& clientId, T&& message)
  {
    // Retrieve the current session using the client ID
    auto session = ctx.getSession(clientId);
    if(!session) {
      IRSOL_LOG_ERROR("No session found for client {}", clientId);
      return {};
    }
    return process(session, std::move(message));
  }

protected:
  HandlerBase(Context ctx): ctx(ctx){};

  virtual std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    T&&                                                     message) = 0;

  Context ctx;
};
}

using AssignmentHandler = internal::HandlerBase<protocol::Assignment>;
using InquiryHandler    = internal::HandlerBase<protocol::Inquiry>;
using CommandHandler    = internal::HandlerBase<protocol::Command>;

}
}
}