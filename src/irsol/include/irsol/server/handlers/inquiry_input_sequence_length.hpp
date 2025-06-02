#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

struct InquiryInputSequenceLength : InquiryHandler
{
  InquiryInputSequenceLength(Context ctx);

  std::vector<out_message_t> operator()(
    const irsol::types::client_id_t& clientId,
    protocol::Inquiry&&              message) override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol