#pragma once
#include "irsol/server/client_session.hpp"
#include <string>
#include <vector>

namespace irsol {
namespace internal {
struct CommandResponse {
  std::string message;
  std::string broadcastMessage = "";
};

class CommandProcessor {
public:
  static std::vector<CommandResponse> handleQuery(const std::string &query, ClientSession &session);
  static std::vector<CommandResponse>
  handleCommand(const std::string &command, const std::string &params, ClientSession &session);
};
} // namespace internal
} // namespace irsol
