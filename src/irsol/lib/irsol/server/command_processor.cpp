#include "irsol/server/command_processor.hpp"
#include "irsol/logging.hpp"
#include <sstream>

namespace irsol {

std::vector<CommandResponse> CommandProcessor::handleQuery(const std::string &query,
                                                           ClientSession &session) {
  auto &camera = session.app().camera();

  std::istringstream iss(query);
  std::string token;
  iss >> token;

  if (token == "camera_status") {
    IRSOL_LOG_DEBUG("Querying camera status");
    return {
        CommandResponse{"fr=10.0"},
        CommandResponse{"frrs=4"},
        CommandResponse{"frc=12.0"},
    };
  }
  if (token == "fps") {
    IRSOL_LOG_DEBUG("Querying frame rate");
    // Broadcast
    return {CommandResponse{"", "fr=4.0"}};
  }

  IRSOL_LOG_ERROR("Unknown query: '{}'", token);
  return {CommandResponse{"ERR Unknown query"}};
}
std::vector<CommandResponse> CommandProcessor::handleCommand(const std::string &cmd,
                                                             ClientSession &session) {
  auto &camera = session.app().camera();

  std::istringstream iss(cmd);
  std::string token;
  iss >> token;

  IRSOL_LOG_DEBUG("Processing command: '{}'", cmd);

  if (token == "SET") {
    std::string param, value;
    iss >> param >> value;
    camera.setParam(param, value);
    return {CommandResponse{"", "UPDATED " + param + " " + value}};
  }
  if (token == "CAPTURE") {
    auto img = camera.captureImage();
    // Send dummy reply or image path
    return {CommandResponse{"CAPTURED"}};
  }

  IRSOL_LOG_ERROR("Unknown command: '{}'", token);
  return {CommandResponse{"ERR Unknown command"}};
}

} // namespace irsol