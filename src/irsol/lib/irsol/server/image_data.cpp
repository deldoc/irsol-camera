#include "irsol/server/image_data.hpp"

namespace irsol {
namespace internal {
ImageData::ImageData(std::shared_ptr<void> d, size_t s,
                     std::chrono::time_point<std::chrono::high_resolution_clock> ts, size_t h,
                     size_t w, size_t c)
    : data(std::move(d)), size(s), timestamp(ts), height(h), width(w), channels(c) {}
} // namespace internal
} // namespace irsol