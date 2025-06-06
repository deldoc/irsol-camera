/// @file examples/04-message-handlers/main.cpp
/// @brief Demonstrates creation, registration, and dispatch of handlers using
/// @ref irsol::server::handlers::MessageHandler.

#include "irsol/irsol.hpp"

#include <memory>
#include <string>
#include <vector>

/// @brief Returns the program name, typically used for logging.
/// If `PROGRAM_NAME` is not defined at compile time, returns `"message-handlers-demo"`.
const std::string
getProgramName()
{
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "message-handlers-demo"
#endif
  return PROGRAM_NAME;
}

/// @brief Example custom handler for Assignment messages.
class MyAssignmentHandler : public irsol::server::handlers::AssignmentHandler
{
public:
  MyAssignmentHandler(std::shared_ptr<irsol::server::handlers::Context> ctx)
    : irsol::server::handlers::AssignmentHandler(ctx)
  {}

  /// Overrides the base operator() as we skip the collection of the client's Session
  /// in this demo, which is the behavior of the base class.
  std::vector<irsol::server::handlers::out_message_t> operator()(
    IRSOL_MAYBE_UNUSED const irsol::types::client_id_t& clientId,
    irsol::protocol::Assignment&&                       message) override
  {
    return process(nullptr, std::move(message));
  }

protected:
  std::vector<irsol::server::handlers::out_message_t> process(
    IRSOL_MAYBE_UNUSED std::shared_ptr<irsol::server::ClientSession> client,
    irsol::protocol::Assignment&&                                    message) override
  {
    IRSOL_LOG_INFO("[MyAssignmentHandler] Called with {}.", message.toString());
    // Respond with a Success message
    // Due to move-only semantics of `Success`, we must create the response vector
    // in this way. Using brace-initialization like `return
    // {irsol::protocol::Success::from(message)};` would not work here.
    std::vector<irsol::server::handlers::out_message_t> result;
    result.emplace_back(irsol::protocol::Success::from(message));
    return result;
  }
};

/// @brief Demonstrates handler creation, registration, and dispatch.
void
demoHandlers()
{
  // Don't pass a Context, as we don't need it in this demo
  std::shared_ptr<irsol::server::handlers::Context> ctx = nullptr;

  // Create a MessageHandler instance
  irsol::server::handlers::MessageHandler msgHandler;

  // Create and register a custom Assignment handler for "foo" identifier.
  msgHandler.registerHandler<irsol::protocol::Assignment>(
    "foo", irsol::server::handlers::makeHandler<MyAssignmentHandler>(ctx));

  // Simulate a session and client id
  irsol::types::client_id_t clientId = irsol::utils::uuid();

  // Dispatch messages to the MessageHandler

  // Create test Assignment messages
  irsol::protocol::Assignment fooMsg("foo", 42);
  IRSOL_LOG_INFO("Dispatching {}...", fooMsg.toString());
  auto fooResult = msgHandler.handle(clientId, std::move(fooMsg));
  IRSOL_LOG_INFO("fooResult size: {}", fooResult.size());
  for(const auto& msg : fooResult) {
    IRSOL_LOG_INFO("Response: {}", irsol::protocol::toString(msg));
  }

  // Try dispatching an unregistered identifier
  irsol::protocol::Assignment unknownMsg("baz", 0);
  IRSOL_LOG_INFO("Dispatching {} (should not find handler)...", unknownMsg.toString());
  auto unknownResult = msgHandler.handle(clientId, std::move(unknownMsg));
  IRSOL_LOG_INFO("unknownResult size: {}", unknownResult.size());
  for(const auto& msg : unknownResult) {
    IRSOL_LOG_INFO("Response: {}", irsol::protocol::toString(msg));
  }
}

/// @brief Main entry point for the handler demo application.
int
main()
{
  // Construct log file path based on program name
  std::string logPath = "logs/" + getProgramName() + ".log";
  irsol::initLogging(logPath.c_str());

  // Enable custom assertion handler
  irsol::initAssertHandler();

  demoHandlers();
  return 0;
}
