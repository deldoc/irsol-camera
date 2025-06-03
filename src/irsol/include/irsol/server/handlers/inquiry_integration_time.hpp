#pragma once

#include "irsol/server/handlers/base.hpp"

namespace irsol {
namespace server {
namespace handlers {

class InquiryIntegrationTimeHandler : public InquiryHandler
{
public:
  InquiryIntegrationTimeHandler(Context ctx);

protected:
  std::vector<out_message_t> process(
    std::shared_ptr<irsol::server::ClientSession> session,
    protocol::Inquiry&&                           message) final override;
};
}  // namespace handlers
}  // namespace server
}  // namespace irsol