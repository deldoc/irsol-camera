#include "irsol/protocol/message/assignment.hpp"

#include "irsol/assert.hpp"
#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {

Assignment::Assignment(const std::string& identifier, irsol::types::protocol_value_t value)
  : identifier(utils::validateIdentifier(identifier)), value(value)
{}

std::string
Assignment::toString() const
{
  std::ostringstream oss;
  oss << "Assignment{"
      << "identifier: '" << identifier << "', value: ";
  if(hasInt()) {
    oss << "<int> " << std::get<int>(value);
  } else if(hasDouble()) {
    oss << "<double> " << std::get<double>(value);
  } else if(hasString()) {
    oss << "<string> \"" << std::get<std::string>(value) << "\"";
  } else {
    IRSOL_ASSERT_ERROR(false, "Invalid assignment value type");
    throw std::runtime_error("Invalid assignment value type");
  }
  oss << '}';
  return oss.str();
}

bool
Assignment::hasInt() const
{
  return std::holds_alternative<int>(value);
}

bool
Assignment::hasDouble() const
{
  return std::holds_alternative<double>(value);
}

bool
Assignment::hasString() const
{
  return std::holds_alternative<std::string>(value);
}
}  // namespace protocol
}  // namespace irsol