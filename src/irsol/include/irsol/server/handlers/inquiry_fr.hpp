#pragma once

#include "irsol/server/handlers/base.hpp"
namespace irsol {
namespace server {
namespace handlers {

struct InquiryFRHandler : InquiryHandler
{
  InquiryFRHandler(Context ctx);

  std::vector<out_message_t> operator()(
    const ::irsol::types::client_id_t& client_id,
    protocol::Inquiry&&                message) override;
};
}
}
}