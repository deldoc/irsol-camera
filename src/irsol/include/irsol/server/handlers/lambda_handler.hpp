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
class LambdaHandler : public internal::HandlerBase<T>
{
  using lambda_function_t =
    std::function<std::vector<out_message_t>(Context&, const irsol::types::client_id_t&, T&&)>;

public:
  LambdaHandler(irsol::server::handlers::Context ctx, lambda_function_t callback)
    : internal::HandlerBase<T>(ctx), m_callback(callback)
  {}

protected:
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    T&&                                           message) final override
  {
    return m_callback(this->ctx, session->id(), std::move(message));
  }

private:
  lambda_function_t m_callback;
};

}
}
}