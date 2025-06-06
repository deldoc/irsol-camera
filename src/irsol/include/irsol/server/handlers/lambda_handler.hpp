/**
 * @file irsol/server/handlers/lambda_handler.hpp
 * @brief Declaration of the LambdaHandler class for protocol message handling.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::LambdaHandler class, which allows
 * protocol message handlers to be implemented as lambda functions or other callable objects.
 *
 * ## When to use LambdaHandler
 *
 * LambdaHandler is ideal for situations where:
 * - You want to quickly implement a handler for a protocol message without creating a new class.
 * - The handler logic is simple, stateless, or only used in a specific context.
 * - You want to register handlers dynamically at runtime, e.g., for testing, scripting, or plugin
 * systems.
 * - You wish to encapsulate handler logic inline, close to where the handler is registered.
 *
 * LambdaHandler is especially useful for:
 * - Prototyping new protocol features.
 * - Writing one-off or ad-hoc handlers for custom commands.
 * - Unit/integration testing with mock handlers.
 * - Reducing boilerplate for trivial handlers.
 *
 * Example usage:
 * ```cpp
 * ctx.registerLambdaHandler<protocol::Command>(
 *   "my_custom_cmd",
 *   ctx,
 *   [](handlers::Context& ctx, const irsol::types::client_id_t& clientId, protocol::Command&& cmd)
 * {
 *     // Custom logic here
 *     std::vector<out_message_t> result;
 *     result.emplace_back(protocol::Success::from(cmd));
 *     return result;
 * );
 * ```
 */

#pragma once

#include "irsol/server/handlers/base.hpp"
#include "irsol/traits.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler class that delegates protocol message processing to a lambda or callable.
 * @tparam T Protocol message type (Assignment, Inquiry, Command).
 * @ingroup Handlers
 *
 * LambdaHandler enables protocol message handlers to be implemented as lambda functions or
 * other callable objects, rather than requiring a dedicated handler class.
 *
 * The lambda receives the handler context, the client ID, and the protocol message.
 */
template<
  typename T,
  std::enable_if_t<irsol::traits::is_type_in_variant<T, irsol::protocol::InMessage>::value, int> =
    0>
class LambdaHandler : public internal::HandlerBase<T>
{
  using lambda_function_t =
    std::function<std::vector<out_message_t>(Context&, const irsol::types::client_id_t&, T&&)>;

public:
  /**
   * @brief Constructs a LambdaHandler with the given context and callback.
   * @param ctx Handler context.
   * @param callback Lambda or callable to invoke for each message.
   */
  LambdaHandler(irsol::server::handlers::Context ctx, lambda_function_t callback)
    : internal::HandlerBase<T>(ctx), m_callback(callback)
  {}

protected:
  /**
   * @brief Processes the protocol message by invoking the stored lambda/callable.
   * @param session The client session.
   * @param message The protocol message.
   * @return Vector of outbound messages (success or error).
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    T&&                                           message) final override
  {
    return m_callback(this->ctx, session->id(), std::move(message));
  }

private:
  /// The lambda or callable to invoke for each message.
  lambda_function_t m_callback;
};

}
}
}