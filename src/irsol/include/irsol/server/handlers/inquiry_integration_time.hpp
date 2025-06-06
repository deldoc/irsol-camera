/**
 * @file irsol/server/handlers/inquiry_integration_time.hpp
 * @brief Declaration of the InquiryIntegrationTimeHandler class.
 * @ingroup Handlers
 *
 * Defines the @ref irsol::server::handlers::InquiryIntegrationTimeHandler class,
 * which handles inquiry messages to retrieve the camera integration time (exposure).
 */

#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

/**
 * @brief Handler for inquiry of the camera integration time (exposure).
 * @ingroup Handlers
 *
 * Processes inquiry messages to retrieve the current camera integration time.
 */
class InquiryIntegrationTimeHandler : public InquiryHandler
{
public:
  /**
   * @brief Constructs the InquiryIntegrationTimeHandler.
   * @param ctx Handler context.
   */
  InquiryIntegrationTimeHandler(std::shared_ptr<Context> ctx);

protected:
  /**
   * @brief Processes an inquiry message to retrieve the integration time.
   * @param session The client session.
   * @param message The inquiry message.
   * @return Vector of outbound messages containing the integration time.
   */
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Inquiry&&                           message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol