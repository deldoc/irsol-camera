
#include "irsol/camera/discovery.hpp"
#include "irsol/logging.hpp"

using namespace std;

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

  FeaturePermissions(bool isAvailable, bool isReadable, bool isWritable)
      : value(FeaturePermissionValue(
            (isAvailable ? FeaturePermissionValue::AVAILABLE
                         : FeaturePermissionValue::NOT_AVAILABLE) |
            (isReadable ? FeaturePermissionValue::READABLE : FeaturePermissionValue::UNSET) |
            (isWritable ? FeaturePermissionValue::WRITABLE : FeaturePermissionValue::UNSET))) {}

  bool isAvailable() const { return value & FeaturePermissionValue::AVAILABLE; }
  bool isReadable() const { return value & FeaturePermissionValue::READABLE; }
  bool isWritable() const { return value & FeaturePermissionValue::WRITABLE; }

  bool operator<(const FeaturePermissions &other) const { return value < other.value; }
};

std::map<FeaturePermissions, std::vector<NeoAPI::Feature *>>
extractCameraFeatures(NeoAPI::Cam &cam) {
  std::map<internal::FeaturePermissions, std::vector<NeoAPI::Feature *>> featurePermissionsMap;

  uint64_t featureCount = 0;
  for (auto &f : cam.GetFeatureList()) {
    IRSOL_LOG_TRACE("Feature: {0:s}", f.GetName().c_str());
    auto isAvailable = f.IsAvailable();
    auto isReadable = f.IsReadable();
    auto isWritable = f.IsWritable();
    internal::FeaturePermissions permissions = {isAvailable, isReadable, isWritable};
    if (featurePermissionsMap.find(permissions) == featurePermissionsMap.cend()) {
      featurePermissionsMap[permissions] = std::vector<NeoAPI::Feature *>();
    }
    featurePermissionsMap[permissions].push_back(&f);
    featureCount++;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable" // Disable unused variable warning
  for (auto &[_, features] : featurePermissionsMap) {
#pragma GCC diagnostic pop // restores the warning
    std::sort(features.begin(), features.end(), [](NeoAPI::Feature *a, NeoAPI::Feature *b) {
      return std::string(a->GetName()) < std::string(b->GetName());
    });
  }

  IRSOL_LOG_INFO("Loaded {0:d} camera features", featureCount);
  return featurePermissionsMap;
}

} // namespace internal

CameraFeatureDiscovery::CameraFeatureDiscovery(CameraInterface &cam) : m_cam(cam) {}

void CameraFeatureDiscovery::run() {
  const auto &featurePermissionsMap = internal::extractCameraFeatures(m_cam.getNeoCam());
  for (const auto &[permissions, features] : featurePermissionsMap) {
    IRSOL_LOG_INFO("Permissions: isAvailable: {0}, isReadable: {1}, isWritable: {2}",
                   permissions.isAvailable(), permissions.isReadable(), permissions.isWritable());
    for (const auto feature : features) {
      std::string featureDescription{feature->GetName().c_str()};
      if (permissions.value & internal::FeaturePermissionValue::AVAILABLE) {
        if (permissions.value & internal::FeaturePermissionValue::READABLE) {
          featureDescription += ", readable (";
          featureDescription += NeoAPI::NeoString(*feature).c_str();
          featureDescription += ")";
        }
        if (permissions.value & internal::FeaturePermissionValue::WRITABLE) {
          featureDescription += ", writable";
        }
      } else {
        featureDescription += ", not available";
      }

      IRSOL_LOG_INFO(" - {0:s}", featureDescription);
    }
  }
}

} // namespace irsol