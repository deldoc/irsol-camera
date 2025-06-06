/**
 * @file irsol/server/handlers/factory.hpp
 * @brief Factory utilities for constructing handler instances.
 * @ingroup Handlers
 *
 * Provides helper functions for creating handler and lambda handler instances.
 */

#pragma once

#include "irsol/server/handlers/context.hpp"
#include "irsol/server/handlers/lambda_handler.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Constructs a handler instance of the given type.
 * @tparam HandlerT Handler class type.
 * @tparam Args Constructor argument types.
 * @param ctx Handler context.
 * @param args Additional arguments for the handler constructor.
 * @return Handler instance.
 */
template<typename HandlerT, typename... Args>
constexpr auto
makeHandler(Context& ctx, Args&&... args)
{
  return HandlerT(ctx, std::forward<Args>(args)...);
}

/**
 * @brief Constructs a lambda handler for the specified message type.
 * @tparam InMessageT Incoming message type.
 * @tparam LambdaT Lambda function type.
 * @param ctx Handler context.
 * @param lambda Lambda function to invoke for the message.
 * @return LambdaHandler instance.
 */
template<typename InMessageT, typename LambdaT>
constexpr auto
makeLambdaHandler(Context& ctx, LambdaT&& lambda)
{
  return LambdaHandler<InMessageT>{
    ctx,
    std::function<std::vector<protocol::OutMessage>(
      Context&, const irsol::types::client_id_t&, InMessageT&&)>(std::forward<LambdaT>(lambda))};
}
}
}
}