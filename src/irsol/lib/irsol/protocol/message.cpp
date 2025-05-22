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

InMessageKind
getMessageKind(const InMessage& msg)
{
  return std::visit(
    [](auto&& value) -> InMessageKind {
      using T = std::decay_t<decltype(value)>;
      if constexpr(std::is_same_v<T, Assignment>)
        return InMessageKind::ASSIGNMENT;
      else if constexpr(std::is_same_v<T, Inquiry>)
        return InMessageKind::INQUIRY;
      else if constexpr(std::is_same_v<T, Command>)
        return InMessageKind::COMMAND;
    },
    msg);
}

OutMessageKind
getMessageKind(const OutMessage& msg)
{
  return std::visit(
    [](auto&& value) -> OutMessageKind {
      using T = std::decay_t<decltype(value)>;
      if constexpr(std::is_same_v<T, Status>)
        return OutMessageKind::STATUS;
      else if constexpr(std::is_same_v<T, Error>)
        return OutMessageKind::ERROR;
    },
    msg);
}

bool
isAssignment(const InMessage& msg)
{
  return std::holds_alternative<Assignment>(msg);
}

bool
isInquiry(const InMessage& msg)
{
  return std::holds_alternative<Inquiry>(msg);
}

bool
isCommand(const InMessage& msg)
{
  return std::holds_alternative<Command>(msg);
}

bool
isStatus(const OutMessage& msg)
{
  return std::holds_alternative<Status>(msg);
}

bool
isError(const OutMessage& msg)
{
  return std::holds_alternative<Error>(msg);
}

}  // namespace protocol
}  // namespace irsol
