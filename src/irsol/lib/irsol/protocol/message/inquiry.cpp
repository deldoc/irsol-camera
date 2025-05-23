#include "irsol/protocol/message/inquiry.hpp"
#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
Inquiry::Inquiry(const std::string& identifier): identifier(utils::validateIdentifier(identifier))
{}

std::string
Inquiry::toString() const
{
  std::ostringstream oss;
  oss << "Inquiry{"
      << "identifier: " << identifier << "}";
  return oss.str();
}
}  // namespace protocol
}  // namespace irsol