#include "irsol/protocol/parsing/parser.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol/utils.hpp"

#include <regex>

namespace irsol {
namespace protocol {
std::optional<InMessage>
Parser::parse(const std::string& line)
{
  IRSOL_LOG_TRACE("Parsing string '{}' for message", line);
  std::string s = utils::trim(line);
  IRSOL_LOG_TRACE("Trimmed result: '{}'", s);
  std::vector<std::string> errorMessages;
  errorMessages.reserve(3);
  if(auto asg = parseAssignment(s)) {
    IRSOL_LOG_TRACE(
      "String '{}' parsed as assignment message: '{}'", line, asg.getMessage().toString());
    return asg.getMessage();
  } else {
    errorMessages.push_back(asg.getError());
  }
  if(auto inq = parseInquiry(s)) {
    IRSOL_LOG_TRACE("String '{}' parsed as inquiry message: {}", line, inq.getMessage().toString());
    return inq.getMessage();
  } else {
    errorMessages.push_back(inq.getError());
  }
  if(auto cmd = parseCommand(s)) {
    IRSOL_LOG_TRACE("String '{}' parsed as command message: {}", line, cmd.getMessage().toString());
    return cmd.getMessage();
  } else {
    errorMessages.push_back(cmd.getError());
  }

  std::string fullErrorMessage = "";
  for(const auto& errorMessage : errorMessages) {
    fullErrorMessage += errorMessage + "; ";
  }
  IRSOL_LOG_WARN(
    "String '{}' could not be parsed as any known message type. {}", line, fullErrorMessage);
  return std::nullopt;
}

internal::ParserResult<Assignment>
Parser::parseAssignment(const std::string& line)
{
  // An assignment line looks like:
  // <identifier>=<value>
  // where:
  // <identifier> is a string of alphanumeric characters and underscores (no spaces allowed), with
  // optional array-indexing <value> is a string that is dynamically interpreted as an integer,
  // double, or string The '=' is mandatory.

  // Example valid assignment lines:
  // foo=32
  // bar=53.6
  // qux_123=0.432
  // _underscore=43
  // array_like[1]=hello
  // nested_array_like[1][2]=0  -> TODO: multidimensional has commas for indices and not multiple
  // brackets. single_quote='single quote' double_quote="double quote" braces={sting value}
  static const std::regex re(R"(^([a-zA-Z]+[a-zA-Z0-9_]*(?:\[\d+\])*)=(.+)$)");
  std::smatch             m;
  std::string             errorMessage;
  if(std::regex_match(line, m, re)) {
    try {
      return {Assignment{utils::trim(m[1]), parseValue(utils::trim(m[2]))}};
    } catch(const std::invalid_argument& e) {
      errorMessage = e.what();
    }
  } else {
    errorMessage = "Regex pattern for Assignment did not match";
  }
  return {std::move(errorMessage)};
}

internal::ParserResult<Inquiry>
Parser::parseInquiry(const std::string& line)
{
  // An inquiry line looks like:
  // <identifier>?
  // where:
  // <identifier> is a string of alphanumeric characters and underscores (no spaces allowed), with
  // optional array-indexing The '?' is mandatory.

  // Example valid inquiry lines:
  // foo?
  // bar?
  // qux_123?
  // _underscore?
  // array_like[1]?
  // nested_array_like[1][2]?

  static const std::regex re(R"(^([a-zA-Z]+[a-zA-Z0-9_]*(?:\[\d+\])*)\?$)");
  std::smatch             m;
  std::string             errorMessage;
  if(std::regex_match(line, m, re)) {
    try {
      return {Inquiry{utils::trim(m[1])}};
    } catch(const std::invalid_argument& e) {
      errorMessage = e.what();
    }
  } else {
    errorMessage = "Regex pattern for Inquiry did not match";
  }
  return {std::move(errorMessage)};
}

internal::ParserResult<Command>
Parser::parseCommand(const std::string& line)
{
  // A command line looks like:
  // <identifier>
  // where:
  // <identifier> is a string of alphanumeric characters and underscores (no spaces allowed)

  // Example valid command lines:
  // foo
  // bar
  // qux_123
  // _underscore

  static const std::regex re(R"(^([a-zA-Z]+[a-zA-Z0-9_]*)$)");
  std::smatch             m;
  std::string             errorMessage;
  if(std::regex_match(line, m, re)) {
    try {
      return {Command{utils::trim(m[1])}};
    } catch(const std::invalid_argument& e) {
      errorMessage = e.what();
    }
  } else {
    errorMessage = "Regex pattern for Command did not match";
  }
  return {std::move(errorMessage)};
}

irsol::types::protocol_value_t
Parser::parseValue(const std::string& valStr)
{
  // Try double first (more general)
  try {
    double d = utils::fromString<double>(valStr);

    // Heuristic: if the string contains a '.' or 'e'/'E', treat it as double
    // This allows to parse a value such as '5.0' as a double, not an int.
    if(
      valStr.find('.') != std::string::npos || valStr.find('e') != std::string::npos ||
      valStr.find('E') != std::string::npos) {
      return d;
    }

    // Otherwise, treat as int if within range
    if(d >= std::numeric_limits<int>::min() && d <= std::numeric_limits<int>::max()) {
      return static_cast<int>(d);
    }

    return d;
  } catch(...) {
    // Fall through
  }

  // Else, string (strip quotes/braces if needed)
  if(
    !valStr.empty() && ((valStr.front() == '"' && valStr.back() == '"') ||
                        (valStr.front() == '\'' && valStr.back() == '\'') ||
                        (valStr.front() == '{' && valStr.back() == '}'))) {
    return valStr.substr(1, valStr.size() - 2);
  }

  return valStr;
}

}
}