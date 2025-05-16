#pragma once

namespace irsol {

class CommandHandler {
public:
    static void process_command(const char* command, int client_socket);
};

} // namespace irsol
