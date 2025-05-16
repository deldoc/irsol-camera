#pragma once

#include "neoapi/neoapi.hpp"
#include <chrono>
#include <mutex>
#include <thread>

namespace irsol {

class CameraStatusMonitor {
public:
  CameraStatusMonitor(const NeoAPI::Cam &cam,
                      std::chrono::milliseconds monitorInterval = std::chrono::milliseconds(100));
  ~CameraStatusMonitor();
  void start();
  void stop();

protected:
  virtual void runMonitor() const;

private:
  const NeoAPI::Cam &m_cam;
  std::chrono::milliseconds m_monitorInterval;

  std::mutex m_startStopMutex;
  bool m_hasStartedMonitor;

  std::thread m_monitorThread;
};
} // namespace irsol