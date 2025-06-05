/**
 * @file irsol/camera/discovery.hpp
 * @brief Utility function for discovering the features that are available in a camera.
 *
 * See @ref interacting_with_camera_features for an example.
 */

#pragma once

#include "irsol/camera/interface.hpp"

#include <map>
#include <vector>

namespace irsol {
namespace camera {
namespace internal {

/**
 * @brief Enumeration for feature permission bit flags.
 *
 * Represents the availability and access permissions of a camera feature.
 */
enum FeaturePermissionValue
{
  UNSET         = 0,          ///< Permission state not set.
  NOT_AVAILABLE = (1u << 0),  ///< Feature is not available.
  AVAILABLE     = (1u << 1),  ///< Feature is available.
  READABLE      = (1u << 2),  ///< Feature is readable.
  WRITABLE      = (1u << 3)   ///< Feature is writable.
};

/**
 * @brief Encapsulates the permissions of a camera feature.
 *
 * Stores permission flags indicating availability, readability, and writability.
 * Provides convenience methods to query individual permissions.
 */
struct FeaturePermissions
{
  FeaturePermissionValue value;  ///< Combined permission flags.

  /**
   * @brief Constructs FeaturePermissions from boolean flags.
   *
   * @param isAvailable Whether the feature is available.
   * @param isReadable Whether the feature is readable.
   * @param isWritable Whether the feature is writable.
   */
  FeaturePermissions(bool isAvailable, bool isReadable, bool isWritable)
    : value(FeaturePermissionValue(
        (isAvailable ? FeaturePermissionValue::AVAILABLE : FeaturePermissionValue::NOT_AVAILABLE) |
        (isReadable ? FeaturePermissionValue::READABLE : FeaturePermissionValue::UNSET) |
        (isWritable ? FeaturePermissionValue::WRITABLE : FeaturePermissionValue::UNSET)))
  {}

  /**
   * @brief Checks if the feature is available.
   * @return true if available, false otherwise.
   */
  inline bool isAvailable() const
  {
    return value & FeaturePermissionValue::AVAILABLE;
  }

  /**
   * @brief Checks if the feature is readable.
   * @return true if readable, false otherwise.
   */
  inline bool isReadable() const
  {
    return value & FeaturePermissionValue::READABLE;
  }

  /**
   * @brief Checks if the feature is writable.
   * @return true if writable, false otherwise.
   */
  inline bool isWritable() const
  {
    return value & FeaturePermissionValue::WRITABLE;
  }

  /**
   * @brief Comparison operator for ordering FeaturePermissions.
   *
   * Enables usage as a key in ordered containers like std::map.
   *
   * @param other The other FeaturePermissions to compare with.
   * @return true if this permission value is less than the other.
   */
  inline bool operator<(const FeaturePermissions& other) const
  {
    return value < other.value;
  }
};

/**
 * @brief Extracts and groups camera features by their permissions.
 *
 * Queries the given NeoAPI camera instance for features,
 * categorizes them by their permission sets, and returns a map
 * from FeaturePermissions to lists of corresponding feature pointers.
 *
 * @param cam NeoAPI camera object to query.
 * @return std::map mapping FeaturePermissions to vectors of NeoAPI::Feature pointers.
 */
std::map<FeaturePermissions, std::vector<NeoAPI::Feature*>> extractCameraFeatures(NeoAPI::Cam& cam);

}  // namespace internal

/**
 * @brief Discovers all camera features and their permissions.
 *
 * Uses internal utilities to query the camera interface and
 * report on the available, readable, and writable features.
 *
 * @param cam Reference to the camera interface to query.
 * @returns A string representing the discovered features formatted as a table.
 */
std::string discoverCameraFeatures(Interface& cam);

}  // namespace camera
}  // namespace irsol
