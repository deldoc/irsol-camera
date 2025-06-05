/**
 * @file irsol/camera/pixel_format.hpp
 * @brief Utilities for managing and transforming pixel formats between each other.
 */

#pragma once

#include <algorithm>
#include <stdint.h>

namespace irsol {

namespace camera {

/**
 * @brief Helper trait to map a pixel bit depth to an appropriate integral representation type.
 *
 * This trait is specialized for supported bit depths. Attempting to use an unsupported
 * bit depth will cause a compilation error due to the lack of a default definition.
 *
 * Usage:
 * ```cpp
 * using Rep8  = PixelRepresentation<8>::type;   // uint8_t
 * using Rep12 = PixelRepresentation<12>::type;  // uint16_t
 * using Rep16 = PixelRepresentation<16>::type;  // uint16_t
 * ```
 *
 * @tparam BitDepth Number of bits per pixel.
 */
template<uint8_t BitDepth>
struct PixelRepresentation;  // no default definition on purpose

/**
 * @brief Specialization for 8-bit pixel depth.
 */
template<>
struct PixelRepresentation<8>
{
  using type = uint8_t;
};

/**
 * @brief Specialization for 12-bit pixel depth.
 *
 * Since 12 bits do not fit into standard types cleanly,
 * we use a 16-bit unsigned integer type to hold pixel data.
 */
template<>
struct PixelRepresentation<12>
{
  using type = uint16_t;
};

/**
 * @brief Specialization for 16-bit pixel depth.
 *
 * Uses a 16-bit unsigned integer type to hold pixel data.
 */
template<>
struct PixelRepresentation<16>
{
  using type = uint16_t;
};

/**
 * @brief Represents a pixel with a given bit depth.
 *
 * Provides the underlying integral representation type for the pixel data,
 * along with compile-time constants for minimum and maximum representable values.
 *
 * @tparam BitDepth Number of bits per pixel.
 */
template<uint8_t BitDepth>
struct Pixel
{
  /// The integral type used to represent pixel values at this bit depth.
  using representation = typename PixelRepresentation<BitDepth>::type;

  /**
   * @brief Returns the maximum representable pixel value for this bit depth.
   *
   * Computed as (2^BitDepth) - 1.
   *
   * @return Maximum pixel value.
   */
  static constexpr uint64_t max()
  {
    return (1ULL << BitDepth) - 1;
  }

  /**
   * @brief Returns the minimum representable pixel value (always zero).
   *
   * @return Minimum pixel value.
   */
  static constexpr uint64_t min()
  {
    return 0;
  }
};

/**
 * @brief Helper to convert pixel values from one bit depth to another with scaling.
 *
 * This template computes a scaling factor at compile time to convert pixel values
 * from SourceBitDepth to TargetBitDepth by linearly mapping the value ranges.
 *
 * Example usage:
 * ```cpp
 * using SourcePixel = Pixel<12>;
 * using TargetPixel = Pixel<16>;
 * auto scaledValue = PixelConversion<12, 16>::scale(sourceValue);
 * ```
 *
 * @tparam SourceBitDepth Bit depth of the input pixel value.
 * @tparam TargetBitDepth Bit depth of the output pixel value.
 */
template<uint8_t SourceBitDepth, uint8_t TargetBitDepth>
struct PixelConversion
{
  /// Compile-time scaling factor: max(Target) / max(Source)
  static constexpr double factor =
    static_cast<double>(Pixel<TargetBitDepth>::max()) / Pixel<SourceBitDepth>::max();

  /**
   * @brief Scales a pixel value from source bit depth to target bit depth.
   *
   * Performs linear scaling of the input value and clamps the result
   * to the maximum value allowed by the target bit depth.
   *
   * @param value Pixel value at SourceBitDepth.
   * @return Scaled pixel value at TargetBitDepth.
   */
  static typename Pixel<TargetBitDepth>::representation scale(
    typename Pixel<SourceBitDepth>::representation value)
  {
    auto rawScale = factor * value;
    return static_cast<typename Pixel<TargetBitDepth>::representation>(
      std::min(rawScale, static_cast<decltype(rawScale)>(Pixel<TargetBitDepth>::max())));
  }
};

}  // namespace camera
}  // namespace irsol
