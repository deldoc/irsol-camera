#include "irsol/server/image_data.hpp"

#include "irsol/types.hpp"

namespace irsol {
namespace server {
namespace internal {
ImageData::ImageData(
  std::vector<irsol::types::byte_t>&& data,
  irsol::types::timepoint_t           ts,
  size_t                              h,
  size_t                              w,
  size_t                              c,
  size_t                              imageId)
  : data(std::move(data)), timestamp(ts), height(h), width(w), channels(c), imageId(imageId)
{}
}  // namespace internal
}  // namespace server
}  // namespace irsol