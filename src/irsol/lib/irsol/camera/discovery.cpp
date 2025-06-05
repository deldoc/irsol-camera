
#include "irsol/camera/discovery.hpp"

#include "irsol/logging.hpp"
#include "irsol/macros.hpp"

#include <tabulate/table.hpp>

using namespace std;

namespace irsol {
namespace camera {
namespace internal {
std::map<FeaturePermissions, std::vector<NeoAPI::Feature*>>
extractCameraFeatures(NeoAPI::Cam& cam)
{
  std::map<internal::FeaturePermissions, std::vector<NeoAPI::Feature*>> featurePermissionsMap;

  uint64_t featureCount = 0;
  for(auto& f : cam.GetFeatureList()) {
    IRSOL_LOG_TRACE("Feature: {0:s}", f.GetName().c_str());
    auto                         isAvailable = f.IsAvailable();
    auto                         isReadable  = f.IsReadable();
    auto                         isWritable  = f.IsWritable();
    internal::FeaturePermissions permissions = {isAvailable, isReadable, isWritable};
    if(featurePermissionsMap.find(permissions) == featurePermissionsMap.cend()) {
      featurePermissionsMap[permissions] = std::vector<NeoAPI::Feature*>();
    }
    featurePermissionsMap[permissions].push_back(&f);
    featureCount++;
  }

  IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START  // Disable unused variable warning
    for(auto& [_, features] : featurePermissionsMap)
  {
    IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END  // restores the warning
      std::sort(features.begin(), features.end(), [](NeoAPI::Feature* a, NeoAPI::Feature* b) {
        return std::string(a->GetName()) < std::string(b->GetName());
      });
  }

  IRSOL_LOG_INFO("Loaded {0:d} camera features", featureCount);
  return featurePermissionsMap;
}
}
std::string
discoverCameraFeatures(irsol::camera::Interface& cam)
{
  const auto& featurePermissionsMap =
    irsol::camera::internal::extractCameraFeatures(cam.getNeoCam());
  tabulate::Table table;
  table.add_row({"Feature Name", "Available", "Readable", "Writable"});

  for(const auto& [permissions, features] : featurePermissionsMap) {
    for(const auto* feature : features) {
      table.add_row({feature->GetName().c_str(),
                     permissions.isAvailable() ? "yes" : "no",
                     permissions.isReadable() ? "yes" : "no",
                     permissions.isWritable() ? "yes" : "no"});
    }
  }

  // Optional formatting:
  table.column(0).format().font_align(tabulate::FontAlign::right);
  table.column(1).format().font_align(tabulate::FontAlign::center);
  table.column(2).format().font_align(tabulate::FontAlign::center);
  table.column(3).format().font_align(tabulate::FontAlign::center);

  return table.str();
}
}  // namespace camera
}  // namespace irsol