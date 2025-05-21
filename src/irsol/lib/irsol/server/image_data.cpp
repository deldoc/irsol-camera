#include "irsol/server/image_data.hpp"

namespace irsol {
namespace server {
namespace internal {
ImageData::ImageData(
  std::shared_ptr<void>                              d,
  size_t                                             s,
  std::chrono::time_point<std::chrono::steady_clock> ts,
  size_t                                             h,
  size_t                                             w,
  size_t                                             c,
  size_t                                             imageId)
  : data(std::move(d)), size(s), timestamp(ts), height(h), width(w), channels(c), imageId(imageId)
{}
}  // namespace internal
}  // namespace server
}  // namespace irsol