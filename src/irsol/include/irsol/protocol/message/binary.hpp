#pragma once

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/types.hpp"

#include <array>
#include <numeric>
#include <sstream>
#include <vector>

namespace irsol {
namespace protocol {

/**
 * @brief Represents a binary data attribute in the protocol.
 */
struct BinaryDataAttribute
{
  BinaryDataAttribute(const std::string& identifier, irsol::types::protocol_value_t value);

  /// The identifier associated with the binary data attribute. Must start with a character,
  /// followed by alphanumeric characters and underscores.
  std::string identifier;

  /// The value associated with the binary data attribute.
  irsol::types::protocol_value_t value;

  /**
   * @brief Converts the binary data attribute to a string.
   * @return A string representation of the binary data attribute (e.g., "x=42").
   */
  std::string toString() const;

  /// @return true if the value is of type int.
  bool hasInt() const;

  /// @return true if the value is of type double.
  bool hasDouble() const;

  /// @return true if the value is of type string.
  bool hasString() const;
};

namespace internal {
template<typename T, std::uint8_t N>
struct BinaryData
{
  IRSOL_STATIC_ASSERT(N >= 1, "Binary data dimensionality must be at least 1");

  /// @brief  The dimensionality of the binary data.
  static constexpr uint8_t dim = N;
  /// @brief  The type of the binary data.
  using value_t = T;
  /// @brief  The number of bytes required to store a value of type T.
  static constexpr uint16_t value_t_bytes = sizeof(value_t);

  /**
   * @brief Constructs a binary data object.
   * @param data Owning pointer to the binary data.
   * @param shape Shape of the binary data.
   * @param attributes Additional attributes associated with the binary data.
   * @note data and attributes are moved from the input to the constructed object.
   */
  BinaryData(
    std::vector<T>&&                   data,
    const std::array<uint64_t, N>&     shape,
    std::vector<BinaryDataAttribute>&& attributes = {})
    : data(std::move(data))
    , shape(shape)
    , numElements(std::accumulate(shape.begin(), shape.end(), 1ull, std::multiplies<>{}))
    , numBytes(numElements * value_t_bytes)
    , attributes(std::move(attributes))
  {
    IRSOL_LOG_TRACE("BinaryData constructed: {}", toString());
    IRSOL_ASSERT_ERROR(
      this->data.size() == this->numElements,
      "Data size (%lu) does not match the number of elements (%lu)",
      this->data.size(),
      this->numElements);
  }

  // Move constructor.
  BinaryData(BinaryData&& other) = default;

  // Delete the copy-constructor to prevent accidental copying of binary data attributes.
  BinaryData(const BinaryData&) = delete;
  // Delete the assignment operator to prevent accidental assignment of binary data attributes.
  BinaryData& operator=(const BinaryData&) = delete;
  // Delete the move-assignment operator to prevent accidental moving of binary data attributes.
  BinaryData& operator=(BinaryData&& other) noexcept = delete;

  /// Owning pointer to the binary data.
  std::vector<T> data;
  /// Shape of the binary data.
  std::array<uint64_t, dim> shape;
  /// Number of elements in the binary data.
  uint64_t numElements;
  /// Number of bytes required to store the binary data.
  uint64_t numBytes;
  /// Additional attributes associated with the binary data.
  std::vector<BinaryDataAttribute> attributes;

  /**
   * @brief Converts the binary data to a string.
   * @return A string representation of the binary data attribute.
   */
  std::string toString() const
  {
    std::stringstream ss;
    ss << "BinaryData[shape=(" << std::to_string(shape[0]);
    for(uint8_t i = 1; i < dim; ++i) {
      ss << "x" << std::to_string(shape[i]);
    }
    ss << ")](" << std::to_string(numBytes) << " bytes"
       << ")";
    return ss.str();
  }
};
}

using BinaryDataBuffer     = internal::BinaryData<irsol::types::byte_t, 1>;
using ImageBinaryData      = internal::BinaryData<irsol::types::byte_t, 2>;
using ColorImageBinaryData = internal::BinaryData<irsol::types::byte_t, 3>;
}
}