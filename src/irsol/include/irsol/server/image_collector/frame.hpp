#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/protocol.hpp"

namespace irsol {
namespace server {
namespace frame_collector {

struct FrameMetadata
{
  irsol::types::timepoint_t timestamp;
  uint64_t                  frameId;
};

/**
 * Represents data captured from the FrameCollector, and distributed to clients.
 */
struct Frame
{
  FrameMetadata                    metadata;
  irsol::protocol::ImageBinaryData image;

  Frame(FrameMetadata metadata, irsol::protocol::ImageBinaryData&& data)
    : metadata(metadata), image(std::move(data))
  {}
  Frame(const Frame& other)     = delete;
  Frame(Frame&& other) noexcept = default;
  Frame& operator=(const Frame& other) = delete;
  Frame& operator=(Frame&& other) noexcept = default;
};
}
}
}