/**
 * @file irsol/server/message_handler.hpp
 * @brief Message routing layer between protocol and application logic.
 * @ingroup Handlers
 *
 * @details
 * This file defines the `MessageHandler` class, which acts as the glue between parsed
 * @ref irsol::protocol::InMessage protocol messages and application-defined logic callbacks.
 *
 * It allows registering handler functions for three categories of messages:
 * - Assignments: Used for state updates (e.g., setting parameters).
 * - Inquiries: Used to query state or configuration.
 * - Commands: Used to trigger actions or behaviors.
 *
 * This class is used by the @ref irsol::server::App to delegate protocol message handling
 * to specific client logic based on the message type and identifier.
 */

#pragma once

#include "irsol/macros.hpp"
#include "irsol/protocol.hpp"
#include "irsol/traits.hpp"
#include "irsol/types.hpp"

#include <string>
#include <unordered_map>
#include <utility>

namespace irsol {
namespace server {

namespace handlers {

/**
 * @brief Binds incoming protocol messages to the appropriate per-client logic.
 *
 * This class acts as a central dispatcher used by the application layer @ref irsol::server::App
 * to process structured incoming messages @ref irsol::protocol::InMessage. Each message is handled
 * by a user-registered callback function based on its type and identifier.
 *
 * Messages are categorized into three types:
 * - @ref irsol::protocol::Assignment (e.g., parameter changes)
 * - @ref irsol::protocol::Inquiry (e.g., state queries)
 * - @ref irsol::protocol::Command (e.g., actions or triggers)
 *
 * For each type, client-specific logic is registered using `registerHandler()`.
 * When a message arrives, `handle()` invokes the matching callback and returns
 * the generated response messages to be sent back to the client.
 *
 */
class MessageHandler
{
  using handling_function_response_t = std::vector<protocol::OutMessage>;
  using handler_identifier_t         = std::string;

  template<typename T>
  using handler_function_t =
    std::function<handling_function_response_t(const irsol::types::client_id_t&, T&&)>;

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
  /**
   * @brief Dispatches an incoming message to the correct user-defined handler.
   *
   * This method extracts the message type and identifier from the parsed `InMessage`,
   * then finds the corresponding callback registered via `registerHandler()`.
   * The appropriate handler is invoked with the `clientId` and the message payload.
   *
   * @param clientId Unique identifier of the client sending the message.
   * @param message The parsed incoming message to handle.
   * @return A list of `OutMessage` responses to be sent back to the client.
   */
  handling_function_response_t handle(
    const irsol::types::client_id_t& clientId,
    protocol::InMessage&&            message) const;

  /**
   * @brief Registers a user-defined handler for a specific message type and identifier.
   *
   * Handlers must be registered per message identifier (e.g., `"start_stream"`, `"set_param"`),
   * and per message kind (Assignment, Inquiry, Command). Duplicate registrations are rejected.
   *
   * @tparam T One of `protocol::Assignment`, `protocol::Inquiry`, or `protocol::Command`.
   * @param identifier The string identifier of the message (e.g., the `name` field).
   * @param handler A callable accepting `(client_id_t, T&&)` and returning response messages.
   * @return `true` if the handler was successfully registered, `false` if a duplicate exists.
   */
  template<
    typename T,
    std::enable_if_t<irsol::traits::is_type_in_variant<T, irsol::protocol::InMessage>::value, int> =
      0>
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
      IRSOL_LOG_INFO("Registering handler for 'Assignment(\"{}\")'", identifier);
      m_assignmentMessageHandlers[identifier] = handler;
    } else if constexpr(std::is_same_v<T, protocol::Inquiry>) {
      IRSOL_LOG_INFO("Registering handler for 'Inquiry(\"{}\")'", identifier);
      m_inquiryMessageHandlers[identifier] = handler;
    } else if constexpr(std::is_same_v<T, protocol::Command>) {
      IRSOL_LOG_INFO("Registering handler for 'Command(\"{}\")'", identifier);
      m_commandMessageHandlers[identifier] = handler;
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "registerHandler()");
    }
    return true;
  }

private:
  /**
   * @brief Locates a registered handler (of any message type) for a given message.
   * @param msg The message whose identifier and type are used for lookup.
   * @return A matching handler wrapped in a `std::variant`, or `std::nullopt` if not found.
   */
  std::optional<any_handler_function_t> findHandlerForMessage(const protocol::InMessage& msg) const;

  /**
   * @brief Locates a handler of a specific message type and identifier.
   *
   * @tparam T Message type (`Assignment`, `Inquiry`, or `Command`).
   * @param identifier Identifier of the message to match.
   * @return A callable if found, or `std::nullopt` if no handler is registered.
   */
  template<
    typename T,
    std::enable_if_t<irsol::traits::is_type_in_variant<T, irsol::protocol::InMessage>::value, int> =
      0>
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

  /// Registered handlers for `Assignment` messages, keyed by identifier.
  assignment_message_handler_map_t m_assignmentMessageHandlers{};

  /// Registered handlers for `Inquiry` messages, keyed by identifier.
  inquiry_message_handler_map_t m_inquiryMessageHandlers{};

  /// Registered handlers for `Command` messages, keyed by identifier.
  command_message_handler_map_t m_commandMessageHandlers{};
};

}  // namespace handlers
}  // namespace server
}  // namespace irsol
