#pragma once
#include "irsol/protocol/message.hpp"
#include "irsol/protocol/parsing/parser_result.hpp"

#include <optional>
#include <string>

namespace irsol {
namespace protocol {

class Parser
{
public:
  /*
   * Parse a line into a message.
   *
   * @param line The input line to parse.
   * @return An optional containing the parsed message, or nullopt if the line cannot be parsed.
   */
  static std::optional<InMessage> parse(const std::string& line);

private:
  static internal::ParserResult<Assignment> parseAssignment(const std::string& line);
  static internal::ParserResult<Inquiry>    parseInquiry(const std::string& line);
  static internal::ParserResult<Command>    parseCommand(const std::string& line);

  static internal::value_t parseValue(const std::string& valueString);
};
}  // namespace protocol
}  // namespace irsol
