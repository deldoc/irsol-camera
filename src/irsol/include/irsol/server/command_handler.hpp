#pragma once

namespace irsol {

class CommandHandler {
public:
  static void processCommand(const char *command, int clientSocket);
};

} // namespace irsol
