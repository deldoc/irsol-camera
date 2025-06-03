#pragma once

#include "irsol/macros.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/handlers/base.hpp"

#include <string_view>
namespace irsol {
namespace server {
namespace handlers {
namespace internal {

struct InquiryCameraImageAttributeLeft
{
  static constexpr std::string_view name = "OffsetX";
};

struct InquiryCameraImageAttributeWidth
{
  static constexpr std::string_view name = "Width";
};

struct InquiryCameraImageAttributeTop
{
  static constexpr std::string_view name = "OffsetY";
};

struct InquiryCameraImageAttributeHeight
{
  static constexpr std::string_view name = "Height";
};

template<typename InquiryCameraImageAttribute>
class InquiryImgHandlerBase : public InquiryHandler
{
  static constexpr std::string_view name = InquiryCameraImageAttribute::name;

public:
  InquiryImgHandlerBase(Context ctx): InquiryHandler(ctx) {}

protected:
  std::vector<out_message_t> process(
    IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Inquiry&&                                              message) final override
  {
    auto&                      cam   = ctx.app.camera();
    int                        value = cam.getParam<int>(std::string(name));
    std::vector<out_message_t> result;
    result.emplace_back(protocol::Success::from(message, irsol::types::protocol_value_t{value}));
    return result;
  }
};
}  // namespace internal

using InquiryImgLeftHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeLeft>;
using InquiryImgTopHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeTop>;
using InquiryImgWidthHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeWidth>;
using InquiryImgHeightHandler =
  internal::InquiryImgHandlerBase<internal::InquiryCameraImageAttributeHeight>;

}  // namespace handlers
}  // namespace server
}  // namespace irsol