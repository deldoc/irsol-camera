#include "neoapi/neoapi.hpp"

namespace irsol {
namespace utils {
void log_camera_info(const NeoAPI::CamInfo& info);
NeoAPI::CamInfoList& discover_cameras();
}
} // namespace irsol