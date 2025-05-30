#include "irsol/queue.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("SafeQueue<T>::done())", "[SafeQueue]")
{
  auto queue = irsol::utils::SafeQueue<int>(3);
  CHECK_FALSE(queue.done());
  queue.producerFinished();
  CHECK(queue.done());
}

TEST_CASE("SafeQueue<T>::sizes", "[SafeQueue]")
{
  auto queue = irsol::utils::SafeQueue<int>(3);
  CHECK(queue.empty());
  for(size_t i = 0; i < 3; ++i) {
    CHECK_FALSE(queue.full());
    CHECK(queue.size() == i);
    queue.push(i);
    CHECK(queue.size() == i + 1);
  }
  CHECK(queue.full());
  CHECK_FALSE(queue.empty());

  int value;
  for(size_t i = 0; i < 3; ++i) {
    CHECK_FALSE(queue.empty());
    CHECK(queue.pop(value));
    CHECK(value == static_cast<int>(i));
    CHECK(queue.size() == 2 - i);
  }
  CHECK(queue.empty());
  CHECK_FALSE(queue.full());
}
