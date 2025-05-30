#include "irsol/irsol.hpp"

#include <chrono>
int
main()
{

  irsol::initLogging("log/simple.log");
  irsol::initAssertHandler();

  IRSOL_LOG_DEBUG("Starting simple example");

  irsol::camera::Interface cam;
  cam.setExposure(std::chrono::seconds(1));
  IRSOL_LOG_INFO("Exposure time set to {}", irsol::utils::durationToString(cam.getExposure()));

  irsol::server::frame_collector::FrameCollector collector{cam};
  collector.start();

  auto frameQueue = std::make_shared<
    irsol::utils::SafeQueue<std::unique_ptr<irsol::server::frame_collector::Frame>>>();
  collector.registerClient("client_1", 1.0, frameQueue, 5);

  std::unique_ptr<irsol::server::frame_collector::Frame> framePtr;
  while(!frameQueue->done() && frameQueue->pop(framePtr)) {
    IRSOL_LOG_DEBUG(
      "Frame received from client_1: {}, captured at {}, ID {}",
      framePtr->image.toString(),
      irsol::utils::timestampToString(framePtr->metadata.timestamp),
      framePtr->metadata.frameId);
  }

  return 0;
}