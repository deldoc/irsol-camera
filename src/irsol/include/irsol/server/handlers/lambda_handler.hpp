#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

template<
  typename T,
  std::enable_if_t<::irsol::protocol::traits::IsInMessageVariant<T>::value, int> = 0>
struct LambdaHandler : public internal::HandlerBase<T>
{
  using lambda_function_t =
    std::function<std::vector<out_message_t>(Context&, const ::irsol::types::client_id_t&, T&&)>;

  LambdaHandler(::irsol::server::handlers::Context ctx, lambda_function_t callback)
    : internal::HandlerBase<T>(ctx), m_callback(callback)
  {}

  std::vector<out_message_t> operator()(const ::irsol::types::client_id_t& client_id, T&& message)
    override
  {
    return m_callback(this->ctx, client_id, std::move(message));
  }

private:
  lambda_function_t m_callback;
};

}
}
}