/**
 * @file irsol/server/handlers/inquiry_frame_rate.hpp
 * @brief Declaration of the InquiryFrameRateHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::InquiryFrameRateHandler class,
 * which handles inquiry messages to retrieve the current frame rate.
 */

#pragma once

#include "irsol/server/handlers/base.hpp"
namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for inquiry of the current frame rate parameter.
 * @ingroup Handlers
 *
 * Processes inquiry messages to retrieve the current frame rate setting.
 */
class InquiryFrameRateHandler : public InquiryHandler
{
public:
  /**
   * @brief Constructs the InquiryFrameRateHandler.
   * @param ctx Handler context.
   */
  InquiryFrameRateHandler(std::shared_ptr<Context> ctx);

protected:
  /**
   * @brief Processes an inquiry message to retrieve the current frame rate.
   * @param session The client session.
   * @param message The inquiry message.
   * @return Vector of outbound messages containing the frame rate.
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Inquiry&&                           message) final override;
};
}
}
}