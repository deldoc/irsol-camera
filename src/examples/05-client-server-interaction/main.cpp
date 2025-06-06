/// @file examples/05-client-server-interaction/main.cpp
/// @brief Demonstrates a sample client-server interaction using sockets.
///
/// @note
/// - This example requires a camera to be physically connected to the running computer.
///   The initialization of the server App and the execution of some commands depend on camera
///   availability.
/// - Some commands (e.g., setting the integration time with `it=number`) will cause the server
///   to broadcast a confirmation message to all connected clients. As a result, each client may
///   receive more messages than it sends.
///
/// Build system integration is expected to define `PROGRAM_NAME` for logging.

#include "irsol/irsol.hpp"
#include "sockpp/tcp_acceptor.h"
#include "sockpp/tcp_connector.h"

#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <vector>

const std::string
getProgramName()
{
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "client-server-interaction-demo"
#endif
  return PROGRAM_NAME;
}

/// @brief Demonstrates handler creation, registration, and dispatch.
std::thread
runServerThread(irsol::server::App& app)
{

  return std::thread([&app]() -> void {
    if(!app.start()) {
      IRSOL_LOG_FATAL("Failed to start server.");
    }
  });
}

/// @brief The function executed by each client thread.
void
clientThreadBody(
  std::string              clientName,
  irsol::types::port_t     port,
  std::vector<std::string> commands)
{
  std::random_device              rd;
  std::mt19937                    gen(rd());
  std::uniform_int_distribution<> waitDist(40, 1000);  // ms

  // Connect to the server at localhost (it is running on the same machine, in a different thread)
  std::error_code           ec;
  std::string               host = "127.0.0.1";
  irsol::types::connector_t conn({host, port}, ec);
  if(ec) {
    IRSOL_LOG_WARN("Failed to connect to server at {}:{}: {}", host, port, ec.message());
    return;
  }
  IRSOL_NAMED_LOG_INFO(clientName, "Connected to server");

  for(const auto& command : commands) {
    auto waitMs = waitDist(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
    IRSOL_NAMED_LOG_INFO(clientName, "Sending command: '{}'", command);
    conn.write(command.data(), command.size());

    char buf[256];
    auto result = conn.read(buf, sizeof(buf) - 1);
    if(result.value() > 0) {
      buf[result.value()] = '\0';
      IRSOL_NAMED_LOG_INFO(clientName, "Received reply: {}", buf);
    } else {
      IRSOL_NAMED_LOG_WARN(clientName, "No reply or connection closed");
      break;
    }
  }
  IRSOL_NAMED_LOG_INFO(clientName, "Client done");
}

/// @brief TCP client that connects to the server and sends commands at random intervals.
std::thread
runClientThread(
  const std::string&             clientName,
  irsol::types::port_t           port,
  const std::vector<std::string> commands)
{
  return std::thread(clientThreadBody, clientName, port, commands);
}

/// @brief Command-line parameters for the program.
struct Params
{
  /// Number of clients to run concurrently (default: 1)
  uint32_t numClients{1};
};

/// @brief Parses command-line arguments using `args` library.
///
/// Supported arguments:
/// - `--clients`, `-c`: Set number of clients to run concurrently
/// - `--help`, `-h`: Show help message and exit
///
/// @param argc Argument count
/// @param argv Argument vector
/// @return Filled `Params` struct
Params
getParams(int argc, char** argv)
{
  args::ArgumentParser parser(getProgramName());
  args::HelpFlag       help(parser, "help", "Display this help menu", {'h', "help"});

  args::ValueFlag<uint32_t> numClientsArg(
    parser, "#clients", "Number of clients to run concurrently", {'c', "clients"});

  try {
    parser.ParseCLI(argc, argv);
  } catch(args::Help) {
    std::cout << parser.Help() << std::endl;
    std::exit(0);
  } catch(args::ParseError& e) {
    std::cerr << "Error parsing command-line arguments:\n" << e.what() << "\n\n" << parser.Help();
    std::exit(1);
  }

  Params params{};
  if(numClientsArg) {
    params.numClients = args::get(numClientsArg);
  }
  return params;
}

void
runDemo(uint32_t numClients)
{
  irsol::types::port_t port = 12345;
  irsol::server::App   app(port);

  // Start server thread
  std::thread serverThread = runServerThread(app);

  // Start multiple clients
  std::vector<std::thread> clientThreads;
  std::vector<std::string> sampleCommands = {"fr?\n",
                                             "it=300\n",
                                             "fr=10.0\n",
                                             "isl=20\n",
                                             "isl?\n",
                                             "fr=0.5\n",
                                             "it=500\n",
                                             "fr?\n",
                                             "it=1000\n"};
  for(size_t i = 0; i < numClients; ++i) {
    std::string clientName = "Client" + std::to_string(i + 1);
    // Create a shuffled copy of sampleCommands for each client
    std::vector<std::string> shuffledCommands = sampleCommands;
    std::shuffle(
      shuffledCommands.begin(), shuffledCommands.end(), std::mt19937(std::random_device()()));
    clientThreads.emplace_back(runClientThread(clientName, port, shuffledCommands));
  }

  // Wait for all clients to finish
  for(auto& t : clientThreads) {
    if(t.joinable())
      t.join();
  }

  // Stop server and wait for it to finish
  app.stop();
  if(serverThread.joinable())
    serverThread.join();
}

/// @brief Main entry point for the handler demo application.
int
main(int argc, char** argv)
{
  // Parse command-line parameters
  Params params = getParams(argc, argv);

  // Construct log file path based on program name
  std::string logPath = "logs/" + getProgramName() + ".log";
  irsol::initLogging(logPath.c_str());

  // Enable custom assertion handler
  irsol::initAssertHandler();

  runDemo(params.numClients);
  return 0;
}
