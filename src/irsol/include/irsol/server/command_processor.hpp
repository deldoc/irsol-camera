#pragma once
#include <memory>
#include <string>
#include <vector>

namespace irsol {
namespace server {
namespace internal {

// Forward declaration
class ClientSession;

struct BinaryData {
  std::shared_ptr<void> data{};
  size_t size{};
};
struct CommandResponse {
  std::string message;
  BinaryData binaryData{};
  std::string broadcastMessage{};
};

class CommandProcessor {
public:
  static std::vector<CommandResponse> handleQuery(const std::string &query,
                                                  std::shared_ptr<ClientSession> session);
  static std::vector<CommandResponse> handleCommand(const std::string &command,
                                                    const std::string &params,
                                                    std::shared_ptr<ClientSession> session);
};
} // namespace internal
} // namespace server
} // namespace irsol
