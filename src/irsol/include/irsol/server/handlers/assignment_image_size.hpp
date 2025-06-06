/**
 * @file irsol/server/handlers/assignment_image_size.hpp
 * @brief Declaration of handlers for assigning camera image size and offset parameters.
 * @ingroup Handlers
 *
 * Defines handler templates and type aliases for assigning camera image region parameters
 * (OffsetX, OffsetY, Width, Height) via assignment messages.
 */

#pragma once

#include "irsol/macros.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/handlers/base.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <string_view>
namespace irsol {
namespace server {
namespace handlers {
namespace internal {

/**
 * @brief Tag type for the camera image left offset parameter ("OffsetX").
 */
struct AssignmentCameraImageAttributeLeft
{
  static constexpr std::string_view name = "OffsetX";
};

/**
 * @brief Tag type for the camera image width parameter ("Width").
 */
struct AssignmentCameraImageAttributeWidth
{
  static constexpr std::string_view name = "Width";
};

/**
 * @brief Tag type for the camera image top offset parameter ("OffsetY").
 */
struct AssignmentCameraImageAttributeTop
{
  static constexpr std::string_view name = "OffsetY";
};

/**
 * @brief Tag type for the camera image height parameter ("Height").
 */
struct AssignmentCameraImageAttributeHeight
{
  static constexpr std::string_view name = "Height";
};

/**
 * @brief Base handler template for assigning a camera image region parameter.
 * @tparam AssignmentCameraImageAttribute Tag type specifying the parameter name.
 * @ingroup Handlers
 *
 * Handles assignment messages to set a specific camera image region parameter.
 */
template<typename AssignmentCameraImageAttribute>
class AssignmentImgHandlerBase : public AssignmentHandler
{
  static constexpr std::string_view name = AssignmentCameraImageAttribute::name;

public:
  /**
   * @brief Constructs the handler.
   * @param ctx Handler context.
   */
  AssignmentImgHandlerBase(Context ctx): AssignmentHandler(ctx) {}

protected:
  /**
   * @brief Processes an assignment message to set the camera image parameter.
   * @param session The client session.
   * @param message The assignment message.
   * @return Vector of outbound messages (success or error).
   */
  std::vector<out_message_t> process(
    IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Assignment&&                                           message) final override
  {
    auto& cam      = ctx.app.camera();
    auto  resValue = cam.setParam(std::string(name), irsol::utils::toInt(message.value));
    std::vector<out_message_t> result;

    // Update the message value with the resulting value after setting the camera parameter.
    // In this way, the resulting value is included in the response message that we broadcast to all
    // clients.
    message.value = irsol::types::protocol_value_t{resValue};
    ctx.broadcastMessage(protocol::Success::from(message));
    return {};
  }
};
}  // namespace internal

/**
 * @brief Handler for assigning the left offset ("OffsetX") of the camera image.
 */
using AssignmentImgLeftHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeLeft>;
/**
 * @brief Handler for assigning the top offset ("OffsetY") of the camera image.
 */
using AssignmentImgTopHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeTop>;
/**
 * @brief Handler for assigning the width ("Width") of the camera image.
 */
using AssignmentImgWidthHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeWidth>;
/**
 * @brief Handler for assigning the height ("Height") of the camera image.
 */
using AssignmentImgHeightHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeHeight>;

}  // namespace handlers
}  // namespace server
}  // namespace irsol