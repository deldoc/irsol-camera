#include "irsol/server/command_handler.hpp"
#include "irsol/logging.hpp"

namespace irsol {

void CommandHandler::processCommand(const char* command, int clientSocket) {
    IRSOL_LOG_DEBUG("Processing command: {}", command);

    // Example command handling
    if (strcmp(command, "PING") == 0) {
        const char* response = "PONG\n";
        write(clientSocket, response, strlen(response));
    } else {
        IRSOL_LOG_WARN("Unknown command: {}", command);
        const char* response = "ERROR: Unknown command\n";
        write(clientSocket, response, strlen(response));
    }
}

} // namespace irsol
