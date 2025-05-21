#pragma once
#include <memory>
#include <string>
#include <vector>

namespace irsol {
namespace server {
namespace internal {

// Forward declaration
class ClientSession;

struct BinaryData
{
  std::shared_ptr<void> data{};
  size_t                size{};
};
struct CommandResponse
{
  std::string message;
  BinaryData  binaryData{};
  std::string broadcastMessage{};
};

class CommandProcessor
{
public:
  using responses_t = std::vector<CommandResponse>;
  static responses_t handleQuery(const std::string& query, std::shared_ptr<ClientSession> session);
  static responses_t handleCommand(
    const std::string&             command,
    const std::string&             params,
    std::shared_ptr<ClientSession> session);
};
}  // namespace internal
}  // namespace server
}  // namespace irsol
