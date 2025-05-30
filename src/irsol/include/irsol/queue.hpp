#pragma once

#include "irsol/assert.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace irsol {
namespace utils {
template<typename T>
class SafeQueue
{
public:
  explicit SafeQueue(size_t max_size = 0): m_maxSize(max_size), m_done(false) {}

  // Disable copy
  SafeQueue(const SafeQueue&) = delete;
  SafeQueue& operator=(const SafeQueue&) = delete;

  // Add an item to the queue (blocking if full and bounded)
  void push(T&& item)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    IRSOL_ASSERT_ERROR(
      m_maxSize == 0 || m_queue.size() < m_maxSize,
      "SafeQueue::push() called when queue is full (max size reached)");
    IRSOL_ASSERT_ERROR(!m_done, "SafeQueue::push() called on an already done queue");

    m_producerConditionVariable.wait(
      lock, [&]() { return m_done || m_maxSize == 0 || m_queue.size() < m_maxSize; });

    if(m_done)
      return;

    m_queue.push(std::move(item));
    m_consumerConditionVariable.notify_one();
  }

  // Try to pop an item. Blocks if empty.
  bool pop(T& out)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    IRSOL_ASSERT_ERROR(!m_done, "SafeQueue::pop() called on an already done queue");

    m_consumerConditionVariable.wait(lock, [&]() { return m_done || !m_queue.empty(); });

    if(m_queue.empty())
      return false;  // done() was called

    out = std::move(m_queue.front());
    m_queue.pop();
    m_producerConditionVariable.notify_one();
    return true;
  }

  // Signal that the producer is done (wakes up consumer)
  void producerFinished()
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    IRSOL_ASSERT_ERROR(!m_done, "SafeQueue::producerFinished() called on an already done queue");

    m_done = true;
    m_consumerConditionVariable.notify_all();
    m_producerConditionVariable.notify_all();
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
  }

  bool full() const
  {
    if(m_maxSize == 0) {
      return false;
    }
    return m_queue.size() >= m_maxSize;
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }

  bool done() const
  {
    return m_done;
  }

private:
  mutable std::mutex      m_mutex;
  std::condition_variable m_producerConditionVariable;
  std::condition_variable m_consumerConditionVariable;
  std::queue<T>           m_queue;
  size_t                  m_maxSize;  // 0 means unbounded queue
  bool                    m_done;
};
}  // namespace utils
}  // namespace irsol
