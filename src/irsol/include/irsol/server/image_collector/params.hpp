#pragma once

#include "irsol/server/image_collector/frame.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <chrono>
#include <functional>
#include <memory>

namespace irsol {
namespace server {
namespace frame_collector {

struct ClientCollectionParams
{
  using frame_queue_t = irsol::utils::SafeQueue<std::unique_ptr<Frame>>;

  double                         fps = 0.0;
  std::chrono::microseconds      interval;
  irsol::types::timepoint_t      nextFrameDue;
  std::shared_ptr<frame_queue_t> queue;
  int64_t                        remainingFrames = -1;     // -1 for infinite
  bool                           immediate       = false;  // true for "immediate-once" clients

  ClientCollectionParams(
    double                         fps,
    std::chrono::microseconds      interval,
    irsol::types::timepoint_t      nextFrameDue,
    std::shared_ptr<frame_queue_t> queue,
    int64_t                        remainingFrames,
    bool                           immediate)
    : fps(fps)
    , interval(interval)
    , nextFrameDue(nextFrameDue)
    , queue(queue)
    , remainingFrames(remainingFrames)
    , immediate(immediate)
  {}
};
}
}
}