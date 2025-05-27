#pragma once

#include "irsol/types.hpp"

#include <string>
#include <vector>

namespace irsol {
namespace protocol {

struct SerializedMessage
{
  std::string                       header;
  std::vector<irsol::types::byte_t> payload{};

  SerializedMessage(const std::string& header, std::vector<irsol::types::byte_t>&& payload);
  SerializedMessage(SerializedMessage&&) noexcept = default;
  SerializedMessage& operator=(SerializedMessage&&) noexcept = default;

  // Delete copy constructor and copy assignment
  SerializedMessage(const SerializedMessage&) = delete;
  SerializedMessage& operator=(const SerializedMessage&) = delete;

  const irsol::types::byte_t* headerData() const;

  size_t headerSize() const;

  const irsol::types::byte_t* payloadData() const;

  size_t payloadSize() const;

  std::string toString() const;
};
}  // namespace protocol
}  // namespace irsol
