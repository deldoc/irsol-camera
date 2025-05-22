#include "irsol/protocol/message.hpp"

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/protocol/types.hpp"
#include "irsol/protocol/utils.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace irsol {
namespace protocol {

std::string
Assignment::toString() const
{
  std::ostringstream oss;
  oss << "Assignment{"
      << "identifier: " << identifier << ", value: ";
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

std::string
Inquiry::toString() const
{
  std::ostringstream oss;
  oss << "Inquiry{"
      << "identifier: " << identifier << "}";
  return oss.str();
}

std::string
Command::toString() const
{
  std::ostringstream oss;
  oss << "Command{"
      << "identifier: " << identifier << "}";
  return oss.str();
}

std::string
Status::toString() const
{
  std::ostringstream oss;
  oss << "Status{"
      << "identifier: " << identifier;
  if(hasBody()) {
    oss << ", body: " << *body;
  }
  oss << "}";
  return oss.str();
}

bool
Status::hasBody() const
{
  return body.has_value();
}

std::string
Error::toString() const
{
  std::ostringstream oss;
  oss << "Error{"
      << "identifier: " << identifier << ", description: " << description << "}";
  return oss.str();
}

}  // namespace protocol
}  // namespace irsol
