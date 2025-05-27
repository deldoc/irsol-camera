#include "irsol/server/image_data.hpp"

namespace irsol {
namespace server {
namespace internal {
ImageData::ImageData(
  std::vector<irsol::types::byte_t>&&                data,
  std::chrono::time_point<std::chrono::steady_clock> ts,
  size_t                                             h,
  size_t                                             w,
  size_t                                             c,
  size_t                                             imageId)
  : data(std::move(data)), timestamp(ts), height(h), width(w), channels(c), imageId(imageId)
{}
}  // namespace internal
}  // namespace server
}  // namespace irsol