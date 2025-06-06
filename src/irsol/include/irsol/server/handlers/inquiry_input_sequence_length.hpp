/**
 * @file irsol/server/handlers/inquiry_input_sequence_length.hpp
 * @brief Declaration of the InquiryInputSequenceLengthHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::InquiryInputSequenceLengthHandler class,
 * which handles inquiry messages to retrieve the input sequence length.
 */

#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for inquiry of the input sequence length parameter.
 * @ingroup Handlers
 *
 * Processes inquiry messages to retrieve the current input sequence length setting.
 */
class InquiryInputSequenceLengthHandler : public InquiryHandler
{
public:
  /**
   * @brief Constructs the InquiryInputSequenceLengthHandler.
   * @param ctx Handler context.
   */
  InquiryInputSequenceLengthHandler(std::shared_ptr<Context> ctx);

protected:
  /**
   * @brief Processes an inquiry message to retrieve the input sequence length.
   * @param session The client session.
   * @param message The inquiry message.
   * @return Vector of outbound messages containing the input sequence length.
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Inquiry&&                           message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol