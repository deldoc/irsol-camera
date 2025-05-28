#include "irsol/protocol/message/variants.hpp"

#include "irsol/protocol/message/assignment.hpp"
#include "irsol/protocol/message/binary.hpp"
#include "irsol/protocol/message/command.hpp"
#include "irsol/protocol/message/error.hpp"
#include "irsol/protocol/message/inquiry.hpp"
#include "irsol/protocol/message/success.hpp"
#include "irsol/protocol/utils.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace irsol {
namespace protocol {
std::string
toString(const InMessage& msg)
{
  if(isAssignment(msg)) {
    return std::get<Assignment>(msg).toString();
  } else if(isInquiry(msg)) {
    return std::get<Inquiry>(msg).toString();
  } else if(isCommand(msg)) {
    return std::get<Command>(msg).toString();
  } else {
    IRSOL_ASSERT_FALSE("Unknown in message type");
    throw std::invalid_argument("Unknown in message type");
  }
}

std::string
toString(const OutMessage& msg)
{
  if(isSuccess(msg)) {
    return std::get<Success>(msg).toString();
  } else if(isError(msg)) {
    return std::get<Error>(msg).toString();
  } else if(isBinaryDataBuffer(msg)) {
    return std::get<BinaryDataBuffer>(msg).toString();
  } else if(isImageBinaryData(msg)) {
    return std::get<ImageBinaryData>(msg).toString();
  } else if(isColorImageBinaryData(msg)) {
    return std::get<ColorImageBinaryData>(msg).toString();
  } else {
    IRSOL_ASSERT_FALSE("Unknown out message type");
    throw std::invalid_argument("Unknown out message type");
  }
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
isSuccess(const OutMessage& msg)
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
