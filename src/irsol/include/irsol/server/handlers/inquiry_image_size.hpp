/**
 * @file irsol/server/handlers/inquiry_image_size.hpp
 * @brief Declaration of handlers for inquiring camera image size and offset parameters.
 * @ingroup Handlers
 *
 * Defines handler templates and type aliases for inquiring camera image region parameters
 * (OffsetX, OffsetY, Width, Height) via inquiry messages.
 */

#pragma once

#include "irsol/macros.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/handlers/base.hpp"

#include <string_view>
namespace irsol {
namespace server {
namespace handlers {
namespace internal {

/**
 * @brief Tag type for the camera image left offset parameter ("OffsetX").
 */
struct InquiryCameraImageAttributeLeft
{
  static constexpr std::string_view name = "OffsetX";
};

/**
 * @brief Tag type for the camera image width parameter ("Width").
 */
struct InquiryCameraImageAttributeWidth
{
  static constexpr std::string_view name = "Width";
};

/**
 * @brief Tag type for the camera image top offset parameter ("OffsetY").
 */
struct InquiryCameraImageAttributeTop
{
  static constexpr std::string_view name = "OffsetY";
};

/**
 * @brief Tag type for the camera image height parameter ("Height").
 */
struct InquiryCameraImageAttributeHeight
{
  static constexpr std::string_view name = "Height";
};

/**
 * @brief Base handler template for inquiring a camera image region parameter.
 * @tparam InquiryCameraImageAttribute Tag type specifying the parameter name.
 * @ingroup Handlers
 *
 * Handles inquiry messages to retrieve a specific camera image region parameter.
 */
template<typename InquiryCameraImageAttribute>
class InquiryImgHandlerBase : public InquiryHandler
{
  static constexpr std::string_view name = InquiryCameraImageAttribute::name;

public:
  /**
   * @brief Constructs the handler.
   * @param ctx Handler context.
   */
  InquiryImgHandlerBase(std::shared_ptr<Context> ctx): InquiryHandler(ctx) {}

protected:
  /**
   * @brief Processes an inquiry message to retrieve the camera image parameter.
   * @param session The client session.
   * @param message The inquiry message.
   * @return Vector of outbound messages containing the parameter value.
   */
  std::vector<out_message_t> process(
    IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Inquiry&&                                              message) final override
  {
    auto&                      cam   = ctx->app.camera();
    int                        value = cam.getParam<int>(std::string(name));
    std::vector<out_message_t> result;
    result.emplace_back(protocol::Success::from(message, irsol::types::protocol_value_t{value}));
    return result;
  }
};
}  // namespace internal

/**
 * @brief Handler for inquiring the left offset ("OffsetX") of the camera image.
 */
using InquiryImgLeftHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeLeft>;
/**
 * @brief Handler for inquiring the top offset ("OffsetY") of the camera image.
 */
using InquiryImgTopHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeTop>;
/**
 * @brief Handler for inquiring the width ("Width") of the camera image.
 */
using InquiryImgWidthHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeWidth>;
/**
 * @brief Handler for inquiring the height ("Height") of the camera image.
 */
using InquiryImgHeightHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeHeight>;

}  // namespace handlers
}  // namespace server
}  // namespace irsol