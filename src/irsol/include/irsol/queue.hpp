/**
 * @file irsol/queue.hpp
 * @brief Thread-safe queue implementation with optional bounded capacity.
 *
 * This header defines the `SafeQueue` template class, which provides
 * a thread-safe queue with blocking push/pop operations and optional
 * maximum size bounding. It is designed for safe communication
 * between one producer and one consumer thread.
 */

#pragma once

#include "irsol/assert.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace irsol {
namespace utils {

/**
 * @class SafeQueue
 * @brief A thread-safe, optionally bounded queue with blocking push and pop operations.
 *
 * This template class implements a producer-consumer queue
 * protected by mutex and condition variables to safely share data between threads.
 * The queue can be bounded by specifying a maximum size or unbounded by leaving
 * the default `max_size` parameter as 0.
 *
 * The queue supports:
 * - Blocking push: waits when full (if bounded) until space becomes available.
 * - Blocking pop: waits when empty until an item is available or the queue is marked done.
 * - Notification when the producer finishes to unblock consumers.
 *
 * @tparam T The type of elements stored in the queue. Must be movable or copyable.
 *
 * @note The queue disables copying to avoid concurrent ownership issues.
 *
 * ```cpp
 * irsol::utils::SafeQueue<int> queue(10); // bounded queue with max size 10
 *
 * // Producer thread
 * std::thread producer([&queue]() {
 *   for (int i = 0; i < 20; ++i) {
 *     queue.push(std::move(i));
 *   }
 *   queue.producerFinished();
 * });
 *
 * // Consumer thread
 * std::thread consumer([&queue]() {
 *   int value;
 *   while (queue.pop(value)) {
 *     std::cout << "Got value: " << value << std::endl;
 *   }
 * });
 *
 * producer.join();
 * consumer.join();
 * ```
 */
template<typename T>
class SafeQueue
{
public:
  /**
   * @brief Constructs a SafeQueue with an optional maximum size.
   * @param max_size The maximum number of elements the queue can hold.
   *                 Zero (default) means the queue is unbounded.
   */
  explicit SafeQueue(size_t max_size = 0): m_maxSize(max_size), m_done(false) {}

  /// Deleted copy constructor to prevent copying.
  SafeQueue(const SafeQueue&) = delete;

  /// Deleted copy assignment operator to prevent copying.
  SafeQueue& operator=(const SafeQueue&) = delete;

  /**
   * @brief Push an item into the queue.
   *
   * Blocks if the queue is bounded and currently full until space becomes available.
   *
   * @param item An rvalue reference to the item to push into the queue.
   *
   * @throws irsol::AssertionException error if the queue is marked done.
   *
   * @note This method uses move semantics to efficiently transfer ownership of the item.
   */
  void push(T&& item)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    IRSOL_ASSERT_ERROR(!m_done, "SafeQueue::push() called on an already done queue");

    m_producerConditionVariable.wait(
      lock, [&]() { return m_done || m_maxSize == 0 || m_queue.size() < m_maxSize; });

    if(m_done)
      return;

    m_queue.push(std::move(item));
    m_consumerConditionVariable.notify_one();
  }

  /**
   * @brief Pop an item from the queue.
   *
   * Blocks if the queue is empty until an item becomes available or the queue
   * is marked done.
   *
   * @param out Reference to a variable where the popped item will be stored.
   * @return `true` if an item was successfully popped, `false` if the queue
   *         is done and empty.
   *
   * @throws irsol::AssertionException error if called after the queue is marked done.
   *
   * @note The popped item is moved to the provided output variable.
   */
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

  /**
   * @brief Signals that the producer has finished producing items.
   *
   * After calling this method, no more items should be pushed.
   * It will unblock any waiting consumers and producers.
   *
   * @throws irsol::AssertionException error if called more than once.
   */
  void producerFinished()
  {
    std::scoped_lock<std::mutex> lock(m_mutex);

    IRSOL_ASSERT_ERROR(!m_done, "SafeQueue::producerFinished() called on an already done queue");

    m_done = true;
    m_consumerConditionVariable.notify_all();
    m_producerConditionVariable.notify_all();
  }

  /**
   * @brief Returns the current size of the queue.
   * @return Number of items currently stored in the queue.
   */
  size_t size() const
  {
    std::scoped_lock<std::mutex> lock(m_mutex);
    return m_queue.size();
  }

  /**
   * @brief Checks if the queue is full.
   * @return `true` if the queue has reached its maximum size (only if bounded).
   *         `false` if unbounded or not full.
   */
  bool full() const
  {
    if(m_maxSize == 0) {
      return false;
    }
    return m_queue.size() >= m_maxSize;
  }

  /**
   * @brief Checks if the queue is empty.
   * @return `true` if the queue contains no items, otherwise `false`.
   */
  bool empty() const
  {
    std::scoped_lock<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }

  /**
   * @brief Returns whether the queue is marked done.
   * @return `true` if the producer has called @ref producerFinished(), otherwise `false`.
   */
  bool done() const
  {
    return m_done;
  }

private:
  mutable std::mutex m_mutex;  ///< Mutex protecting the queue and state.
  std::condition_variable
    m_producerConditionVariable;  ///< Condition variable for producer blocking.
  std::condition_variable
                m_consumerConditionVariable;  ///< Condition variable for consumer blocking.
  std::queue<T> m_queue;                      ///< Underlying queue holding the data.
  size_t        m_maxSize;                    ///< Maximum queue size (0 means unbounded).
  bool          m_done;                       ///< Flag indicating the queue is done.
};

}  // namespace utils
}  // namespace irsol
