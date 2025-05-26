#include "irsol/server/message_handler.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol.hpp"

namespace irsol {
namespace server {
namespace handlers {

MessageHandler::handling_function_response_t
MessageHandler::handle(protocol::InMessage&& message) const
{
  IRSOL_LOG_TRACE("Handling message: '{}'", protocol::toString(message));
  auto handler = findHandlerForMessage(message);
  if(!handler) {
    IRSOL_LOG_ERROR("No handler found for message: '{}'", irsol::protocol::toString(message));
    std::vector<protocol::OutMessage> result;
    result.emplace_back(
      protocol::Error::from(std::move(message), "No handler registered for this message."));
    return result;
  }
  // Dispatch the message to the appropriate handler and return the response
  const std::string messageString = protocol::toString(message);

  try {
    return std::visit(
      [&handler](auto&& msg) -> MessageHandler::handling_function_response_t {
        using T                  = std::decay_t<decltype(msg)>;
        using handler_function_t = MessageHandler::handler_function_t<T>;
        return std::get<handler_function_t>(*handler)(std::move(msg));
      },
      std::move(message));

  } catch(const std::exception& e) {
    IRSOL_LOG_ERROR("Error handling message: '{}': {}", messageString, e.what());
    return {};
  }
}

std::optional<MessageHandler::any_handler_function_t>
MessageHandler::findHandlerForMessage(const protocol::InMessage& msg) const
{
  return std::visit(
    [this](auto&& value) -> std::optional<any_handler_function_t> {
      using T  = std::decay_t<decltype(value)>;
      auto res = this->findHandler<T>(value.identifier);
      if(!res) {
        return std::nullopt;
      }
      return std::make_optional<any_handler_function_t>(*res);
    },
    msg);
}

}  // namespace handlers
}  // namespace server

}  // namespace irsol