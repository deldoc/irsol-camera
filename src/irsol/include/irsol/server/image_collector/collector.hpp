#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/image_collector/frame.hpp"
#include "irsol/server/image_collector/params.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

namespace irsol {
namespace server {
namespace frame_collector {

class FrameCollector
{
public:
  using frame_queue_t = ClientCollectionParams::frame_queue_t;

  FrameCollector(irsol::camera::Interface& camera);
  ~FrameCollector();

  void start();
  void stop();
  bool isBusy() const;

  void registerClient(
    irsol::types::client_id_t      clientId,
    double                         fps,
    std::shared_ptr<frame_queue_t> queue,
    int64_t                        frameCount = -1);
  void deregisterClient(irsol::types::client_id_t clientId);

private:
  static constexpr uint16_t max_queue_size = 10;

  void captureLoop();
  void distributorLoop();

  irsol::camera::Interface& m_cam;

  std::mutex                                                                  m_clientsMutex;
  std::unordered_map<irsol::types::client_id_t, ClientCollectionParams>       m_clients;
  std::map<irsol::types::timepoint_t, std::vector<irsol::types::client_id_t>> m_scheduleMap;

  std::condition_variable m_scheduleCondition;
  std::thread             m_distributorThread;

  std::atomic<bool> m_stopFlag{false};
};
}
}
}