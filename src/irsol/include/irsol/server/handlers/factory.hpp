#pragma once

#include "irsol/server/handlers/context.hpp"
#include "irsol/server/handlers/lambda_handler.hpp"

namespace irsol {
namespace server {
namespace handlers {

template<typename HandlerT, typename... Args>
constexpr auto
makeHandler(Context& ctx, Args&&... args)
{
  return HandlerT(ctx, std::forward<Args>(args)...);
}

template<typename InMessageT, typename LambdaT>
constexpr auto
makeLambdaHandler(Context& ctx, LambdaT&& lambda)
{
  return LambdaHandler<InMessageT>{
    ctx,
    std::function<std::vector<protocol::OutMessage>(
      Context&, const ::irsol::server::client_id_t&, InMessageT&&)>(std::forward<LambdaT>(lambda))};
}
}
}
}