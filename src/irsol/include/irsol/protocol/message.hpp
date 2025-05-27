#pragma once

#include "irsol/assert.hpp"
#include "irsol/protocol/message/in_messages.hpp"
#include "irsol/protocol/message/out_messages.hpp"
#include "irsol/protocol/message/variants.hpp"
#include "irsol/types.hpp"

namespace irsol {
namespace protocol {

inline std::string
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

inline std::string
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

}  // namespace protocol
}