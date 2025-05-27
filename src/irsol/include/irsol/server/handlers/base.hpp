#pragma once

#include "irsol/protocol.hpp"
#include "irsol/server/handlers/context.hpp"

#include <vector>

namespace irsol {
namespace server {

namespace handlers {
using in_message_t  = protocol::InMessage;
using out_message_t = protocol::OutMessage;

namespace internal {

template<
  typename T,
  std::enable_if_t<::irsol::protocol::traits::IsInMessageVariant<T>::value, int> = 0>
struct HandlerBase
{

  HandlerBase(Context ctx): ctx(ctx){};
  virtual std::vector<out_message_t> operator()(
    const ::irsol::types::client_id_t& client_id,
    T&&                                message) = 0;

  Context ctx;
};
}

using AssignmentHandler = internal::HandlerBase<protocol::Assignment>;
using InquiryHandler    = internal::HandlerBase<protocol::Inquiry>;
using CommandHandler    = internal::HandlerBase<protocol::Command>;

}
}
}