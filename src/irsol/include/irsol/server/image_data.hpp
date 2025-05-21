#pragma once

#include <chrono>
#include <memory>

namespace irsol {
namespace server {
namespace internal {
struct ImageData {
  ImageData(std::shared_ptr<void> d, size_t s,
            std::chrono::time_point<std::chrono::steady_clock> ts, size_t h, size_t w, size_t c,
            size_t imageId);
  ImageData() = default;

  std::shared_ptr<void> data;
  size_t size;
  std::chrono::time_point<std::chrono::steady_clock> timestamp;

  size_t height;
  size_t width;
  size_t channels;

  size_t imageId;
};

} // namespace internal
} // namespace server
} // namespace irsol