#pragma once

#include "irsol/server/handlers/base.hpp"
namespace irsol {
namespace server {
namespace handlers {

class InquiryFrameRateHandler : public InquiryHandler
{
public:
  InquiryFrameRateHandler(Context ctx);

protected:
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    protocol::Inquiry&&                                     message) final override;
};
}
}
}