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

Assignment::Assignment(const std::string& identifier, internal::value_t value)
  : identifier(utils::validateIdentifier(identifier)), value(value)
{}

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

Command::Command(const std::string& identifier): identifier(utils::validateIdentifier(identifier))
{}

std::string
Command::toString() const
{
  std::ostringstream oss;
  oss << "Command{"
      << "identifier: " << identifier << "}";
  return oss.str();
}

Status::Status(const std::string& identifier, std::optional<std::string> body)
  : identifier(utils::validateIdentifier(identifier)), body(body)
{}

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

InMessageKind
getInMessageKind(const InMessage& msg)
{
  return std::visit(
    [](auto&& value) -> InMessageKind {
      using T = std::decay_t<decltype(value)>;
      return getInMessageKind<T>(value);
    },
    msg);
}

bool
isAssignment(const InMessage& msg)
{
  return getInMessageKind(msg) == InMessageKind::ASSIGNMENT;
}

bool
isInquiry(const InMessage& msg)
{
  return getInMessageKind(msg) == InMessageKind::INQUIRY;
}

bool
isCommand(const InMessage& msg)
{
  return getInMessageKind(msg) == InMessageKind::COMMAND;
}

OutMessageKind
getOutMessageKind(const OutMessage& msg)
{
  return std::visit(
    [](auto&& value) -> OutMessageKind {
      using T = std::decay_t<decltype(value)>;
      return getOutMessageKind<T>(value);
    },
    msg);
}

bool
isStatus(const OutMessage& msg)
{
  return getOutMessageKind(msg) == OutMessageKind::STATUS;
}

bool
isBinaryDataBuffer(const OutMessage& msg)
{
  return getOutMessageKind(msg) == OutMessageKind::BINARY_BUFFER;
}

bool
isImageBinaryData(const OutMessage& msg)
{
  return getOutMessageKind(msg) == OutMessageKind::BW_IMAGE;
}

bool
isColorImageBinaryData(const OutMessage& msg)
{
  return getOutMessageKind(msg) == OutMessageKind::COLOR_IMAGE;
}
bool
isError(const OutMessage& msg)
{
  return getOutMessageKind(msg) == OutMessageKind::ERROR;
}

}  // namespace protocol
}  // namespace irsol
