#pragma once

#include <chrono>
#include <memory>

namespace irsol {
namespace internal {
struct ImageData {
  ImageData(std::shared_ptr<void> d, size_t s,
            std::chrono::time_point<std::chrono::high_resolution_clock> ts, size_t h, size_t w,
            size_t c);
  ImageData() = default;

  std::shared_ptr<void> data;
  size_t size;
  std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;

  size_t height;
  size_t width;
  size_t channels;
};

} // namespace internal
} // namespace irsol