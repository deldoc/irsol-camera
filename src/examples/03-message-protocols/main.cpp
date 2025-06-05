/// @file examples/02-interacting-with-camera-features/main.cpp
/// @brief Demonstrates basic creation and serialization of protocol messages using
/// `irsol::protocol`.

#include "irsol/irsol.hpp"

/// @brief Returns the program name for logging.
const std::string
getProgramName()
{
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "message-protocols-demo"
#endif
  return PROGRAM_NAME;
}

/// @brief Tries to parse a message string and logs the result or error.
void
tryParse(const std::string& raw)
{
  auto result = irsol::protocol::Parser::parse(raw);
  if(result) {
    IRSOL_LOG_INFO("Parsed '{}': {}", raw, irsol::protocol::toString(*result));
  } else {
    IRSOL_LOG_ERROR("Failed to parse '{}'", raw);
  }
}

/// @brief Demonstrates parsing raw protocol strings into structured InMessages.
void
demoParsing()
{
  IRSOL_LOG_INFO("=== Starting Parsing Demo ===");

  // Valid Assignments with different value types
  tryParse("intVal=42");
  tryParse("floatVal=3.1415");
  tryParse("floatValNegative=-3.1415");
  tryParse("strVal1=\"hello world\"");
  tryParse("strVal2={hello world}");

  // Valid Commands and Inquiries
  tryParse("capture");       // Command
  tryParse("temperature?");  // Inquiry

  // Invalid cases (malformed syntax)
  tryParse("=42");             // Missing identifier
  tryParse("foo==42");         // Extra equals
  tryParse("foo=");            // No value
  tryParse("incomplete[1,2");  // Unterminated array
  tryParse("badcmd!");         // Invalid command
  tryParse("_badcmd");         // Invalid command

  IRSOL_LOG_INFO("=== Parsing Demo Complete ===");
}

/// @brief Demonstrates serializing protocol OutMessages into SerializedMessages
void
demoSerializing()
{
  // Demo 1: Serialize Success from a Command
  irsol::protocol::Command command("demoCommand");
  irsol::protocol::Success successCommand = irsol::protocol::Success::from(command);
  auto serializedCommand = irsol::protocol::Serializer::serialize(std::move(successCommand));
  IRSOL_LOG_INFO("Success(Command): {}", serializedCommand.toString());

  // Demo 2: Serialize Success from an Assignment
  irsol::protocol::Assignment assignment("demoAssignment", irsol::types::protocol_value_t{42});
  irsol::protocol::Success    successAssignment = irsol::protocol::Success::from(assignment);
  auto serializedAssignment = irsol::protocol::Serializer::serialize(std::move(successAssignment));
  IRSOL_LOG_INFO("Success(Assignment): {}", serializedAssignment.toString());

  // Demo 3: Serialize Success from an Inquiry
  irsol::protocol::Inquiry inquiry("demoInquiry");
  irsol::protocol::Success successInquiry =
    irsol::protocol::Success::from(inquiry, irsol::types::protocol_value_t{"inquiry result"});
  auto serializedInquiry = irsol::protocol::Serializer::serialize(std::move(successInquiry));
  IRSOL_LOG_INFO("Success(Inquiry): {}", serializedInquiry.toString());

  // Demo 4: Serialize Error from Command
  irsol::protocol::Error errorCommand = irsol::protocol::Error::from(command, "Command failed");
  auto serializedErrorCommand = irsol::protocol::Serializer::serialize(std::move(errorCommand));
  IRSOL_LOG_INFO("Error(Command): {}", serializedErrorCommand.toString());

  // Demo 5: Serialize Error from Assignment
  irsol::protocol::Error errorAssignment =
    irsol::protocol::Error::from(assignment, "Invalid value");
  auto serializedErrorAssignment =
    irsol::protocol::Serializer::serialize(std::move(errorAssignment));
  IRSOL_LOG_INFO("Error(Assignment): {}", serializedErrorAssignment.toString());

  // Demo 6: Serialize Error from Inquiry
  irsol::protocol::Error errorInquiry = irsol::protocol::Error::from(inquiry, "Not found");
  auto serializedErrorInquiry = irsol::protocol::Serializer::serialize(std::move(errorInquiry));
  IRSOL_LOG_INFO("Error(Inquiry): {}", serializedErrorInquiry.toString());
}

/// @brief Demonstrates protocol message creation and serialization.
void
demoProtocol()
{
  demoParsing();
  demoSerializing();
}

/// @brief Main entry point for the feature demo application.
int
main()
{
  // Initialize logging
  std::string logPath = "logs/" + getProgramName() + ".log";
  irsol::initLogging(logPath.c_str(), spdlog::level::debug);
  irsol::initAssertHandler();

  // Run demo
  demoProtocol();

  return 0;
}
