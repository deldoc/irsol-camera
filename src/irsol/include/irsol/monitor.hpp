#pragma once

#include "neoapi/neoapi.hpp"
#include <mutex>
#include <thread>

namespace irsol {
class CameraStatusMonitor {
public:
  CameraStatusMonitor(const NeoAPI::Cam &cam);
  ~CameraStatusMonitor();
  void start();
  void stop();

protected:
  void run_monitor() const;

private:
  const NeoAPI::Cam &m_cam;

  std::mutex m_startStopMutex;
  bool m_hasStartedMonitor;

  std::thread m_monitorThread;
};
} // namespace irsol