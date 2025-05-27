#pragma once

#include "irsol/types.hpp"

#include <chrono>
#include <memory>
#include <vector>

namespace irsol {
namespace server {
namespace internal {
struct ImageData
{
  ImageData(
    std::vector<irsol::types::byte_t>&&                data,
    std::chrono::time_point<std::chrono::steady_clock> ts,
    size_t                                             h,
    size_t                                             w,
    size_t                                             c,
    size_t                                             imageId);
  ImageData() = default;

  std::vector<irsol::types::byte_t>                  data;
  std::chrono::time_point<std::chrono::steady_clock> timestamp;

  size_t height;
  size_t width;
  size_t channels;

  size_t imageId;
};

}  // namespace internal
}  // namespace server
}  // namespace irsol