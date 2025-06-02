#include "irsol/server/client/state.hpp"

namespace irsol {
namespace server {
namespace internal {

bool
FrameListeningState::running() const
{
  return m_running.load();
}

void
FrameListeningState::stop()
{
  std::scoped_lock<std::mutex> lock(m_threadMutex);
  if(m_stopRequested) {
    m_stopRequested->store(true);
  }
}

}
}
}