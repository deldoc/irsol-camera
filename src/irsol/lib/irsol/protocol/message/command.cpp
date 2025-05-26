#include "irsol/protocol/message/command.hpp"

#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
Command::Command(const std::string& identifier): identifier(utils::validateIdentifier(identifier))
{}

std::string
Command::toString() const
{
  std::ostringstream oss;
  oss << "Command{"
      << "identifier: '" << identifier << "'}";
  return oss.str();
}
}  // namespace protocol
}  // namespace irsol