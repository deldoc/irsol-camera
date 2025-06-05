#include "irsol/irsol.hpp"

#include <chrono>
#include <thread>

void
consume(
  const std::string& name,
  std::shared_ptr<irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>
    queue)
{
  irsol::types::timepoint_t                              t0;
  uint32_t                                               numFrames = 0;
  std::unique_ptr<irsol::server::frame_collector::Frame> framePtr;
  irsol::types::timepoint_t lastImageRetrieved = irsol::types::clock_t::now();
  while(!queue->done() && queue->pop(framePtr)) {
    if(numFrames == 0) {
      t0 = irsol::types::clock_t::now();
    }
    numFrames++;
    auto dt            = irsol::types::clock_t::now() - lastImageRetrieved;
    lastImageRetrieved = irsol::types::clock_t::now();
    IRSOL_NAMED_LOG_WARN(
      name,
      "Frame received from client_1: {}, captured at {}, ID {}, DT: {}",
      framePtr->image.toString(),
      irsol::utils::timestampToString(framePtr->metadata.timestamp),
      framePtr->metadata.frameId,
      irsol::utils::durationToString(dt));
  }
  auto t1 = irsol::types::clock_t::now();
  IRSOL_NAMED_LOG_WARN(
    name,
    "Collected {} frames in {} ({}fps)",
    numFrames,
    irsol::utils::durationToString(t1 - t0),
    static_cast<double>((numFrames - 1) * 1000000) /
      std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
}
int
main()
{

  irsol::initLogging("logs/simple.log");
  irsol::initAssertHandler();

  IRSOL_LOG_DEBUG("Starting simple example");

  irsol::camera::Interface cam;
  cam.setExposure(std::chrono::milliseconds(10));
  IRSOL_LOG_INFO("Exposure time set to {}", irsol::utils::durationToString(cam.getExposure()));

  irsol::server::frame_collector::FrameCollector collector{cam};
  collector.start();

  auto frameQueue1 = std::make_shared<
    irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>();

  auto frameQueue2 = std::make_shared<
    irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>();
  auto frameQueue3 = std::make_shared<
    irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>();
  auto frameQueue4 = std::make_shared<
    irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>();
  auto frameQueue5 = std::make_shared<
    irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>();

  auto t1 = std::thread([frameQueue1]() { consume("Consume1", frameQueue1); });
  auto t2 = std::thread([frameQueue2]() { consume("Consume2", frameQueue2); });
  auto t3 = std::thread([frameQueue3]() { consume("Consume3", frameQueue3); });
  auto t4 = std::thread([frameQueue4]() { consume("Consume4", frameQueue4); });
  auto t5 = std::thread([frameQueue5]() { consume("Consume5", frameQueue5); });

  collector.registerClient("client_1", 16, frameQueue1, 10 * 16);
  collector.registerClient("client_2", 8, frameQueue2, 10 * 8);
  collector.registerClient("client_3", 4, frameQueue3, 10 * 4);
  collector.registerClient("client_4", 2, frameQueue4, 10 * 2);
  collector.registerClient("client_5", 1, frameQueue5, 10 * 1);

  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  return 0;
}