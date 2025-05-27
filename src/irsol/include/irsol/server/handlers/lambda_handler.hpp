#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

namespace internal {

template<
  typename T,
  std::enable_if_t<::irsol::protocol::traits::IsInMessageVariant<T>::value, int> = 0>
struct LambdaHandlerBase : public HandlerBase<T>
{
  using lambda_function_t =
    std::function<std::vector<out_message_t>(Context&, const ::irsol::server::client_id_t&, T&&)>;

  LambdaHandlerBase(::irsol::server::handlers::Context ctx, lambda_function_t callback)
    : HandlerBase<T>(ctx), m_callback(callback)
  {}

  std::vector<out_message_t> operator()(const ::irsol::server::client_id_t& client_id, T&& message)
    override
  {
    return m_callback(this->ctx, client_id, std::move(message));
  }

private:
  lambda_function_t m_callback;
};
}

using AssignmentLambdaHandler = internal::LambdaHandlerBase<protocol::Assignment>;
using InquiryLambdaHandler    = internal::LambdaHandlerBase<protocol::Inquiry>;
using CommandLambdaHandler    = internal::LambdaHandlerBase<protocol::Command>;

}
}
}