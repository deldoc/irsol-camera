
#include "irsol/protocol/message/error.hpp"
#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
 Error::Error(const std::string& identifier, const std::string& description)
  : identifier(utils::validateIdentifier(identifier)), description(description)
{}

std::string
Error::toString() const
{
  std::ostringstream oss;
  oss << "Error{"
      << "identifier: " << identifier << ", description: " << description << "}";
  return oss.str();
}

Error
Error::from(const InMessage& msg, const std::string& description)
{
  return std::visit(
    [&description](auto&& value) -> Error {
      using T = std::decay_t<decltype(value)>;
      return Error::from<T>(value, description);
    },
    msg);
}
}  // namespace protocol
}  // namespace irsol