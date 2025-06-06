/**
 * @file irsol/server/handlers/base.hpp
 * @brief Base handler templates for protocol message processing.
 * @ingroup Handlers
 *
 * Defines generic handler base classes for assignment, inquiry, and command messages,
 * as well as type aliases for common handler types.
 */

#pragma once

#include "irsol/protocol.hpp"
#include "irsol/server/handlers/context.hpp"
#include "irsol/traits.hpp"

#include <vector>

namespace irsol {
namespace server {

namespace handlers {
using in_message_t  = protocol::InMessage;
using out_message_t = protocol::OutMessage;

namespace internal {

/**
 * @brief Generic handler base class for protocol messages.
 * @tparam T Protocol message type (Assignment, Inquiry, Command).
 * @ingroup Handlers
 *
 * Provides the interface for processing protocol messages for a given client session.
 */
template<
  typename T,
  std::enable_if_t<irsol::traits::is_type_in_variant<T, irsol::protocol::InMessage>::value, int> =
    0>
class HandlerBase
{
public:
  /**
   * @brief Invokes the handler for a given client and message.
   * @param clientId The client identifier.
   * @param message The protocol message.
   * @return Vector of outbound messages (success or error).
   *
   * @note This method is invoked by the @ref irsol::server::handlers::MessageHandler::handle
   * method.
   */
  std::vector<out_message_t> operator()(const irsol::types::client_id_t& clientId, T&& message)
  {
    // Retrieve the current session using the client ID
    auto session = ctx.getSession(clientId);
    if(!session) {
      IRSOL_LOG_ERROR("No session found for client {}", clientId);
      return {};
    }
    return process(session, std::move(message));
  }

protected:
  /**
   * @brief Constructs the handler base.
   * @param ctx Handler context.
   */
  HandlerBase(Context ctx): ctx(ctx){};

  /**
   * @brief Processes the protocol message for the given session.
   * @param session The client session.
   * @param message The protocol message.
   * @return Vector of outbound messages (success or error).
   */
  virtual std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    T&&                                           message) = 0;

  /// Handler context (provides access to app and utilities).
  Context ctx;
};
}

/**
 * @brief Handler type for assignment messages.
 */
using AssignmentHandler = internal::HandlerBase<protocol::Assignment>;
/**
 * @brief Handler type for inquiry messages.
 */
using InquiryHandler = internal::HandlerBase<protocol::Inquiry>;
/**
 * @brief Handler type for command messages.
 */
using CommandHandler = internal::HandlerBase<protocol::Command>;

}
}
}