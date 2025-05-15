#include "irsol/discovery.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "neoapi/neoapi.hpp"
#include <map>
#include <tuple>
#include <vector>

namespace irsol {

namespace internal {

enum FeaturePermissionValue {
  UNSET = 0,
  NOT_AVAILABLE = (1u << 0),
  AVAILABLE = (1u << 1),
  READABLE = (1u << 2),
  WRITABLE = (1u << 3)
};

struct FeaturePermissions {
  FeaturePermissionValue value;

  FeaturePermissions(bool is_available, bool is_readable, bool is_writable)
      : value(FeaturePermissionValue(
            (is_available ? FeaturePermissionValue::AVAILABLE
                          : FeaturePermissionValue::NOT_AVAILABLE) |
            (is_readable ? FeaturePermissionValue::READABLE : FeaturePermissionValue::UNSET) |
            (is_writable ? FeaturePermissionValue::WRITABLE : FeaturePermissionValue::UNSET))) {}

  bool is_available() const { return value & FeaturePermissionValue::AVAILABLE; }
  bool is_readable() const { return value & FeaturePermissionValue::READABLE; }
  bool is_writable() const { return value & FeaturePermissionValue::WRITABLE; }

  bool operator<(const FeaturePermissions &other) const { return value < other.value; }
};

std::map<FeaturePermissions, std::vector<NeoAPI::Feature *>>
extract_camera_features(NeoAPI::Cam &cam) {
  std::map<internal::FeaturePermissions, std::vector<NeoAPI::Feature *>> feature_permissions_map;

  uint64_t feature_count = 0;
  for (auto &f : cam.GetFeatureList()) {
    IRSOL_LOG_TRACE("Feature: {0:s}", f.GetName().c_str());
    auto is_available = f.IsAvailable();
    auto is_readable = f.IsReadable();
    auto is_writable = f.IsWritable();
    internal::FeaturePermissions permissions = {is_available, is_readable, is_writable};
    if (feature_permissions_map.find(permissions) == feature_permissions_map.cend()) {
      feature_permissions_map[permissions] = std::vector<NeoAPI::Feature *>();
    }
    feature_permissions_map[permissions].push_back(&f);
    feature_count++;
  }

// Sort the features by name
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable" // Disable unused variable warning
  for (auto &[_, features] : feature_permissions_map) {
#pragma GCC diagnostic pop // restores the warning
    std::sort(features.begin(), features.end(), [](NeoAPI::Feature *a, NeoAPI::Feature *b) {
      return std::string(a->GetName()) < std::string(b->GetName());
    });
  }

  IRSOL_LOG_INFO("Loaded {0:d} camera features", feature_count);
  return feature_permissions_map;
}

} // namespace internal

void discover_camera_features(NeoAPI::Cam &cam) {

  const auto &feature_permissions_map = internal::extract_camera_features(cam);
  for (const auto &[permissions, features] : feature_permissions_map) {
    IRSOL_LOG_INFO("Permissions: is_available: {0}, is_readable: {1}, is_writable: {2}",
                   permissions.is_available(), permissions.is_readable(),
                   permissions.is_writable());
    for (const auto feature : features) {

      std::string feature_description{feature->GetName().c_str()};
      if (permissions.value & internal::FeaturePermissionValue::AVAILABLE) {
        if (permissions.value & internal::FeaturePermissionValue::READABLE) {
          feature_description += ", readable (";
          feature_description += NeoAPI::NeoString(*feature).c_str();
          feature_description += ")";
        }
        if (permissions.value & internal::FeaturePermissionValue::WRITABLE) {
          feature_description += ", writable";
        }
      } else {
        feature_description += ", not available";
      }

      IRSOL_LOG_INFO(" - {0:s}", feature_description);
    }
  }
}

} // namespace irsol