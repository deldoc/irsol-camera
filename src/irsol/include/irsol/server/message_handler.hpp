#pragma once

#include "irsol/protocol.hpp"
#include "irsol/server/types.hpp"

#include <string>
#include <unordered_map>
#include <utility>

namespace irsol {

namespace server {

namespace handlers {

class MessageHandler
{
  using handling_function_response_t = std::vector<protocol::OutMessage>;
  using handler_identifier_t         = std::string;

  template<typename T>
  using handler_function_t =
    std::function<handling_function_response_t(const ::irsol::server::client_id_t&, T&&)>;

  using assignment_handler_function_t = handler_function_t<protocol::Assignment&&>;
  using inquiry_handler_function_t    = handler_function_t<protocol::Inquiry&&>;
  using command_handler_function_t    = handler_function_t<protocol::Command&&>;

  template<typename T>
  using message_handler_map_t = std::unordered_map<handler_identifier_t, T>;

  using assignment_message_handler_map_t = message_handler_map_t<assignment_handler_function_t>;
  using inquiry_message_handler_map_t    = message_handler_map_t<inquiry_handler_function_t>;
  using command_message_handler_map_t    = message_handler_map_t<command_handler_function_t>;

  using any_handler_function_t = std::
    variant<assignment_handler_function_t, inquiry_handler_function_t, command_handler_function_t>;

public:
  handling_function_response_t handle(
    const ::irsol::server::client_id_t& client_id,
    protocol::InMessage&&               message) const;

  template<
    typename T,
    std::enable_if_t<::irsol::protocol::traits::IsInMessageVariant<T>::value, int> = 0>
  bool registerHandler(const std::string& identifier, handler_function_t<T> handler)
  {
    // Make sure there's no duplicate handlers for the same message kind and identifier
    if(auto existingHandler = findHandler<T>(identifier); existingHandler) {
      if constexpr(std::is_same_v<T, protocol::Assignment>) {
        IRSOL_LOG_ERROR(
          "Duplicate handler registered for identifier '{}' and Assignment type", identifier);
      } else if constexpr(std::is_same_v<T, protocol::Inquiry>) {
        IRSOL_LOG_ERROR(
          "Duplicate handler registered for identifier '{}' and Inquiry type", identifier);
      } else if constexpr(std::is_same_v<T, protocol::Command>) {
        IRSOL_LOG_ERROR(
          "Duplicate handler registered for identifier '{}' and Command type", identifier);
      } else {
        IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "registerHandler()");
      }

      return false;
    }

    if constexpr(std::is_same_v<T, protocol::Assignment>) {
      IRSOL_LOG_DEBUG(
        "Registering handler for message kind 'ASSIGNMENT' and identifier '{}'", identifier);
      m_assignmentMessageHandlers[identifier] = handler;
    } else if constexpr(std::is_same_v<T, protocol::Inquiry>) {
      IRSOL_LOG_DEBUG(
        "Registering handler for message kind 'INQUIRY' and identifier '{}'", identifier);
      m_inquiryMessageHandlers[identifier] = handler;
    } else if constexpr(std::is_same_v<T, protocol::Command>) {
      IRSOL_LOG_DEBUG(
        "Registering handler for message kind 'COMMAND' and identifier '{}'", identifier);
      m_commandMessageHandlers[identifier] = handler;
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "registerHandler()");
    }
    return true;
  }

private:
  std::optional<any_handler_function_t> findHandlerForMessage(const protocol::InMessage& msg) const;

  template<
    typename T,
    std::enable_if_t<::irsol::protocol::traits::IsInMessageVariant<T>::value, int> = 0>
  std::optional<handler_function_t<T>> findHandler(const handler_identifier_t& identifier) const
  {
    if constexpr(std::is_same_v<T, protocol::Assignment>) {
      auto it = m_assignmentMessageHandlers.find(identifier);
      return it == m_assignmentMessageHandlers.end() ? std::nullopt
                                                     : std::make_optional(it->second);
    } else if constexpr(std::is_same_v<T, protocol::Inquiry>) {
      auto it = m_inquiryMessageHandlers.find(identifier);
      return it == m_inquiryMessageHandlers.end() ? std::nullopt : std::make_optional(it->second);
    } else if constexpr(std::is_same_v<T, protocol::Command>) {
      auto it = m_commandMessageHandlers.find(identifier);
      return it == m_commandMessageHandlers.end() ? std::nullopt : std::make_optional(it->second);
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "findHandler()");
    }
  }

  assignment_message_handler_map_t m_assignmentMessageHandlers{};
  inquiry_message_handler_map_t    m_inquiryMessageHandlers{};
  command_message_handler_map_t    m_commandMessageHandlers{};
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol