#pragma once

#include "irsol/server/handlers/base.hpp"
namespace irsol {
namespace server {
namespace handlers {

struct InquiryFrameRateHandler : InquiryHandler
{
  InquiryFrameRateHandler(Context ctx);

  std::vector<out_message_t> operator()(protocol::Inquiry&& message) override;
};
}
}
}