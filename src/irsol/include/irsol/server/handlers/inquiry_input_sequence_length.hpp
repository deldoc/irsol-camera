#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct InquiryInputSequenceLengthHandler : InquiryHandler
{
  InquiryInputSequenceLengthHandler(Context ctx);

  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::internal::ClientSession> session,
    protocol::Inquiry&&                                     message) override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol