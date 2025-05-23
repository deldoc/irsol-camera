
#include "irsol/protocol/message/error.hpp"

#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
Error::Error(const std::string& identifier, InMessageKind source, const std::string& description)
  : identifier(utils::validateIdentifier(identifier)), source(source), description(description)
{}

std::string
Error::toString() const
{
  std::ostringstream oss;
  oss << "Error{"
      << "identifier: " << identifier << ", source: " << InMessageKindToString(source)
      << ", description: " << description << "}";
  return oss.str();
}

}  // namespace protocol
}  // namespace irsol