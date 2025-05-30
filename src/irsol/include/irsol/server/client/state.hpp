#pragma once

#include "irsol/types.hpp"

namespace irsol {
namespace server {
namespace internal {
/**
 * @brief Parameters controlling how and when frames are delivered to a client.
 *
 * lastFrameSent: timestamp of the last frame successfully sent to the client.
 * frameRate: desired frame rate (in frames per second) for this client.
 */
struct FrameListeningParams
{
  irsol::types::timepoint_t lastFrameSent{irsol::types::clock_t::now()};
  double                    frameRate;
  int64_t                   numDesiredFrames{-1};  // -1 means unlimited
};

/**
 * @brief Encapsulates all per-client data for managing a user session.
 *
 * A UserSessionData object holds the networking socket for communication,
 * synchronization primitives protecting concurrent access to the socket,
 * and frame delivery parameters that control how images are streamed to the client.
 */
struct UserSessionData
{

  /// Controls the parameters for streaming image frames to the client.
  FrameListeningParams frameListeningParams{};
};

}  // namespace internal
}  // namespace server
}  // namespace irsol