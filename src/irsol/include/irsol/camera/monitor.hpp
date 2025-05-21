#pragma once

#include "irsol/camera/interface.hpp"

#include <chrono>
#include <mutex>
#include <thread>

namespace irsol {
namespace camera {

class StatusMonitor
{
public:
  StatusMonitor(
    const Interface&          cam,
    std::chrono::milliseconds monitorInterval = std::chrono::milliseconds(100));
  ~StatusMonitor();
  void start();
  void stop();

protected:
  virtual void runMonitor() const;

private:
  const Interface&          m_cam;
  std::chrono::milliseconds m_monitorInterval;

  std::mutex m_startStopMutex;
  bool       m_hasStartedMonitor;

  std::thread m_monitorThread;
};
}  // namespace camera
}  // namespace irsol