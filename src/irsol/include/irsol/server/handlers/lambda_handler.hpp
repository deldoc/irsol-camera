#pragma once

#include "irsol/server/handlers/base.hpp"
#include "irsol/traits.hpp"

namespace irsol {
namespace server {
namespace handlers {

template<
  typename T,
  std::enable_if_t<irsol::traits::is_type_in_variant<T, irsol::protocol::InMessage>::value, int> =
    0>
struct LambdaHandler : public internal::HandlerBase<T>
{
  using lambda_function_t =
    std::function<std::vector<out_message_t>(Context&, const irsol::types::client_id_t&, T&&)>;

  LambdaHandler(irsol::server::handlers::Context ctx, lambda_function_t callback)
    : internal::HandlerBase<T>(ctx), m_callback(callback)
  {}

  std::vector<out_message_t> operator()(const irsol::types::client_id_t& clientId, T&& message)
    override
  {
    return m_callback(this->ctx, clientId, std::move(message));
  }

private:
  lambda_function_t m_callback;
};

}
}
}