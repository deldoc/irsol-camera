#include "irsol/protocol/message/variants.hpp"
#include "irsol/protocol/message/assignment.hpp"
#include "irsol/protocol/message/inquiry.hpp"
#include "irsol/protocol/message/command.hpp"
#include "irsol/protocol/message/success.hpp"
#include "irsol/protocol/message/error.hpp"
#include "irsol/protocol/message/binary.hpp"
#include "irsol/protocol/utils.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace irsol {
namespace protocol {


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
