#pragma once

#include "irsol/types.hpp"

#include <thread>

namespace irsol {
namespace server {
namespace internal {

/**
 * @brief Per client parameters for Gis command
 */
struct GisParams
{

  uint64_t inputSequenceLength{16};
  double   frameRate{4.0};
};

/**
 * @brief State associated with the Session client and how it's listening to frames.
 *
 */
struct FrameListeningState
{
  bool        running{false};
  GisParams   gisParams{};
  std::thread listeningThread;
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

  /// Controls the client's state w.r.t listening to frames.
  FrameListeningState frameListeningState{};
};

}  // namespace internal
}  // namespace server
}  // namespace irsol