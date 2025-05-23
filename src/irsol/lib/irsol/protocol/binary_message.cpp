#include "irsol/protocol/binary_message.hpp"

#include "irsol/assert.hpp"
#include "irsol/protocol/types.hpp"
#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
BinaryDataAttribute::BinaryDataAttribute(const std::string& identifier, internal::value_t value)
  : identifier(utils::validateIdentifier(identifier)), value(value)
{}

std::string
BinaryDataAttribute::toString() const
{
  std::ostringstream oss;
  oss << "BinaryDataAttribute{"
      << "identifier: " << identifier << ", value: ";
  if(hasInt()) {
    oss << "<int> " << std::get<int>(value);
  } else if(hasDouble()) {
    oss << "<double> " << std::get<double>(value);
  } else if(hasString()) {
    oss << "<string> \"" << std::get<std::string>(value) << "\"";
  } else {
    IRSOL_ASSERT_ERROR(false, "Invalid binary data attribute value type");
    throw std::runtime_error("Invalid binary data attribute value type");
  }
  oss << '}';
  return oss.str();
}

bool
BinaryDataAttribute::hasInt() const
{
  return std::holds_alternative<int>(value);
}

bool
BinaryDataAttribute::hasDouble() const
{
  return std::holds_alternative<double>(value);
}

bool
BinaryDataAttribute::hasString() const
{
  return std::holds_alternative<std::string>(value);
}

}
}