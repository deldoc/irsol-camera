/**
 * @file
 * @brief Protocol binary data types and attributes definitions.
 *
 * This header defines the structures and templates for representing
 * binary data attributes and binary data buffers of various dimensionalities,
 * including move-only semantics and safety checks on data consistency.
 *
 * @ingroup Protocol
 */
#pragma once

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/macros.hpp"
#include "irsol/types.hpp"

#include <array>
#include <numeric>
#include <sstream>
#include <string_view>
#include <vector>

namespace irsol {
namespace protocol {

/**
 * @ingroup Protocol
 * @brief Represents a single binary data attribute within the protocol.
 *
 * This struct encapsulates a key-value pair where the key is an identifier
 * and the value is a protocol-specific typed value @ref irsol::types::protocol_value_t.
 */
struct BinaryDataAttribute
{
  /**
   * @brief Constructs a BinaryDataAttribute.
   * @param identifier The identifier string.
   * @param value The associated protocol value.
   */
  BinaryDataAttribute(const std::string& identifier, irsol::types::protocol_value_t value);

  /// Identifier of the binary data attribute.
  std::string identifier;

  /// Value associated with the attribute.
  irsol::types::protocol_value_t value;

  /**
   * @brief Returns a string representation of the binary data attribute.
   * @return A string representation of the binary data attribute".
   */
  std::string toString() const;

  /// @return True if the value holds an integer type.
  bool hasInt() const;

  /// @return True if the value holds a double type.
  bool hasDouble() const;

  /// @return True if the value holds a string type.
  bool hasString() const;
};

namespace internal {

/**
 * @ingroup Protocol
 * @brief Helper to get a descriptive name for binary data buffers by dimensionality.
 *
 * @tparam N The dimensionality of the buffer.
 */
template<std::uint8_t N>
struct BinaryDataBufferName
{
  static constexpr std::string_view name()
  {
    if constexpr(N == 1) {
      return "BinaryDataBuffer";
    } else if constexpr(N == 2) {
      return "BinaryDataBuffer2D";
    } else if constexpr(N == 3) {
      return "BinaryDataBuffer3D";
    } else {
      IRSOL_STATIC_UNREACHABLE("Unsupported dimensionality for BinaryDataBuffer");
    }
  }
};

/**
 * @ingroup Protocol
 * @brief Represents a binary data object within the protocol.
 *
 * This class owns a contiguous block of binary data elements and their shape,
 * along with optional additional attributes. It supports only move semantics
 * to avoid unnecessary copies of potentially large data buffers.
 *
 * @tparam NBytes Number of bytes per element (e.g., 1 for 8-bit data, 2 for 16-bit).
 * @tparam N Dimensionality of the binary data (e.g., 2 for images).
 *
 * @note Copy construction and assignment are disabled to prevent expensive copies.
 * Move semantics are supported to allow ownership transfer.
 * Members are non-const to facilitate move operations.
 */
template<std::uint8_t NBytes, std::uint8_t N>
struct BinaryData
{
  IRSOL_STATIC_ASSERT((NBytes == 1 || NBytes == 2), "Binary data element byte size must be 1 or 2");
  IRSOL_STATIC_ASSERT(N >= 1, "Binary data dimensionality must be at least 1");

  /// Number of bytes per element.
  static constexpr uint8_t BYTES_PER_ELEMENT = NBytes;

  /// Dimensionality of the binary data.
  static constexpr uint8_t DIM = N;

  /**
   * @brief Constructs a BinaryData object.
   * @param data Rvalue reference to the binary data bytes; ownership is transferred.
   * @param shape Shape of the data as an array of size N.
   * @param attributes Optional additional attributes; ownership is transferred.
   *
   * @throws irsol::AssertException if the data size does not match shape * bytes per element.
   */
  BinaryData(
    std::vector<irsol::types::byte_t>&& data,
    const std::array<uint64_t, N>&      shape,
    std::vector<BinaryDataAttribute>&&  attributes = {})
    : data(std::move(data))
    , shape(shape)
    , numElements(std::accumulate(shape.begin(), shape.end(), 1ull, std::multiplies<>{}))
    , numBytes(numElements * BYTES_PER_ELEMENT)
    , attributes(std::move(attributes))
  {
    IRSOL_LOG_TRACE("BinaryData constructed: {}", toString());
    IRSOL_ASSERT_ERROR(
      this->data.size() == this->numBytes,
      "Data size (%lu) does not match the number of elements (%lu) multiplied by bytes per element "
      "(%d).",
      this->data.size(),
      this->numElements,
      this->BYTES_PER_ELEMENT);
  }

  /**
   * @brief Move constructs a BinaryData object.
   * @param other The object to move from.
   */
  BinaryData(BinaryData&& other) = default;

  // Disable copy semantics.
  BinaryData(const BinaryData&) = delete;
  BinaryData& operator=(const BinaryData&) = delete;

  // Disable move assignment to avoid accidental reassignment.
  BinaryData& operator=(BinaryData&& other) noexcept = delete;

  /// Owned binary data bytes.
  std::vector<irsol::types::byte_t> data;

  /// Shape of the binary data.
  std::array<uint64_t, DIM> shape;

  /// Total number of elements in the data.
  uint64_t numElements;

  /// Total number of bytes (numElements * bytes per element).
  uint64_t numBytes;

  /// Additional attributes related to the binary data.
  std::vector<BinaryDataAttribute> attributes;

  /**
   * @brief Returns a string summary of the binary data buffer.
   * @return A string describing dimensionality, shape, and size in bytes.
   */
  std::string toString() const
  {
    std::stringstream ss;
    ss << BinaryDataBufferName<N>::name() << "u" << (BYTES_PER_ELEMENT == 1 ? "8" : "16")
       << "[shape=(" << std::to_string(shape[0]);
    for(uint8_t i = 1; i < DIM; ++i) {
      ss << "x" << std::to_string(shape[i]);
    }
    ss << ")](" << std::to_string(numBytes) << " bytes)";
    return ss.str();
  }
};

}  // namespace internal

/**
 * @ingroup Protocol
 * @brief 1-dimensional binary data buffer with 2 bytes per element.
 */
using BinaryDataBuffer = internal::BinaryData<2, 1>;

/**
 * @ingroup Protocol
 * @brief 2-dimensional binary data buffer with 2 bytes per element (e.g., grayscale images).
 */
using ImageBinaryData = internal::BinaryData<2, 2>;

/**
 * @ingroup Protocol
 * @brief 3-dimensional binary data buffer with 2 bytes per element (e.g., color images).
 */
using ColorImageBinaryData = internal::BinaryData<2, 3>;

}  // namespace protocol
}  // namespace irsol
