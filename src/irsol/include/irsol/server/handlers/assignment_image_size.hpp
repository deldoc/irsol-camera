#pragma once

#include "irsol/macros.hpp"
#include "irsol/protocol/message/types.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/handlers/base.hpp"

#include <string_view>
namespace irsol {
namespace server {
namespace handlers {
namespace internal {

struct AssignmentCameraImageAttributeLeft
{
  static constexpr std::string_view name = "OffsetX";
};

struct AssignmentCameraImageAttributeWidth
{
  static constexpr std::string_view name = "Width";
};

struct AssignmentCameraImageAttributeTop
{
  static constexpr std::string_view name = "OffsetY";
};

struct AssignmentCameraImageAttributeHeight
{
  static constexpr std::string_view name = "Height";
};

template<typename AssignmentCameraImageAttribute>
struct AssignmentImgHandlerBase : AssignmentHandler
{
  static constexpr std::string_view name = AssignmentCameraImageAttribute::name;

  AssignmentImgHandlerBase(Context ctx): AssignmentHandler(ctx) {}
  std::vector<out_message_t> operator()(
    IRSOL_MAYBE_UNUSED const ::irsol::server::client_id_t& client_id,
    protocol::Assignment&&                                 message) override
  {
    auto& cam = ctx.app.camera();
    auto  resValue =
      cam.setParam(std::string(name), ::irsol::protocol::internal::toInt(message.value));
    std::vector<out_message_t> result;

    // Update the message value with the resulting value after setting the camera parameter.
    // In this way, the resulting value is included in the response message.
    message.value = ::irsol::protocol::internal::value_t{resValue};
    result.emplace_back(protocol::Success::from(std::move(message)));
    return result;
  }
};
}  // namespace internal

using AssignmentImgLeftHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeLeft>;
using AssignmentImgTopHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeTop>;
using AssignmentImgWidthHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeWidth>;
using AssignmentImgHeightHandler =
  internal::AssignmentImgHandlerBase<internal::AssignmentCameraImageAttributeHeight>;

}  // namespace handlers
}  // namespace server
}  // namespace irsol