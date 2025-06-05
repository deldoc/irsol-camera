/**
 * @file irsol/protocol/parsing/parser.hpp
 * @brief Parses raw protocol input strings into structured messages.
 *
 * Defines the `Parser` class, which is responsible for interpreting textual input from clients
 * and converting it into typed protocol messages, such as @ref irsol::protocol::Assignment,
 * @ref irsol::protocol::Inquiry, or @ref irsol::protocol::Command.
 *
 * @ingroup Protocol
 */

#pragma once

#include "irsol/protocol/message.hpp"
#include "irsol/protocol/parsing/parser_result.hpp"

#include <optional>
#include <string>

namespace irsol {
namespace protocol {

/**
 * @ingroup Protocol
 * @brief Parses raw input strings into structured @ref InMessage instances.
 *
 * The `Parser` provides static methods to convert protocol input lines (e.g., `x=42`, `reset`,
 * `x?`) into well-formed @ref irsol::protocol::InMessage variants. Parsing is designed to be robust
 * and safe, returning an empty result on failure.
 */
class Parser
{
public:
  /**
   * @brief Attempts to parse a single protocol input line into a structured @ref InMessage.
   *
   * This method sequentially tries to interpret the input string as one of the supported message
   * types:
   *
   * - **Assignment** (e.g., `"foo=42"` or `"temp[1]=36.5"`): Key-value bindings with optional
   * indexed identifiers.
   * - **Inquiry** (e.g., `"foo?"` or `"temp[2]?"`): Used to query the current value of an
   * identifier.
   * - **Command** (e.g., `"reset"` or `"shutdown"`): Control messages without arguments.
   *
   * The function internally trims whitespace, applies regular expressions to match expected message
   * formats, and uses helper functions to convert the raw string into a strongly-typed variant.
   *
   * Parsing proceeds in order of decreasing specificity:
   * 1. `parseAssignment()` is attempted first. If it fails, the reason is recorded.
   * 2. `parseInquiry()` is attempted next. If it fails, the reason is recorded.
   * 3. `parseCommand()` is attempted last.
   *
   * If none of the parsers succeed, a warning is logged with details from each parsing attempt,
   * and the method returns `std::nullopt`.
   *
   * @param line A raw protocol line (e.g., from a client or script), which may contain
   * leading/trailing whitespace.
   * @return An optional containing the parsed @ref InMessage variant if successful, or
   * `std::nullopt` on failure.
   *
   * @see parseAssignment
   * @see parseInquiry
   * @see parseCommand
   */
  static std::optional<InMessage> parse(const std::string& line);

private:
  /**
   * @brief Parses an assignment message from a protocol line.
   *
   * Matches lines of the form:
   * ```
   * identifier=value
   * ```
   * where:
   * - `identifier` must begin with a letter and contain alphanumeric characters and underscores.
   * - Array-style indexing is supported, e.g., `array[0]`, `arr[1][2]`.
   * - `value` is interpreted as an integer, double, or string, depending on its format.
   *
   * Valid examples:
   * - `"x=42"`
   * - `"temperature[1]=36.5"`
   * - `"name='Alice'"`
   * - `"meta={some description}"`
   *
   * If the regex matches but value parsing fails, an error string is returned.
   * If the format does not match, a descriptive parsing error is returned.
   *
   * @param line The raw line to parse.
   * @return A ParserResult containing a valid @ref Assignment or an error message.
   */
  static internal::ParserResult<Assignment> parseAssignment(const std::string& line);

  /**
   * @brief Parses an inquiry message from a protocol line.
   *
   * Matches lines of the form:
   * ```
   * identifier?
   * ```
   * where:
   * - `identifier` must begin with a letter and contain alphanumeric characters and underscores.
   * - Array-style indexing is supported (e.g., `array[0]`).
   * - Whitespace is trimmed before matching.
   *
   * Valid examples:
   * - `"x?"`
   * - `"temperature[1]?"`
   * - `"config_setting?"`
   *
   * Returns a valid @ref Inquiry message on success, or a detailed error message on failure.
   *
   * @param line The raw line to parse.
   * @return A ParserResult containing a valid @ref Inquiry or an error message.
   */
  static internal::ParserResult<Inquiry> parseInquiry(const std::string& line);

  /**
   * @brief Parses a command message from a protocol line.
   *
   * Matches lines consisting of a single bare identifier:
   * ```
   * identifier
   * ```
   * where:
   * - `identifier` must start with a letter and contain only alphanumeric characters and
   * underscores.
   * - No parameters or arguments are allowed.
   *
   * Valid examples:
   * - `"reset"`
   * - `"shutdown"`
   * - `"ping"`
   *
   * Returns a @ref Command object on success, or an error message on failure.
   *
   * @param line The raw line to parse.
   * @return A ParserResult containing a valid @ref Command or an error message.
   */
  static internal::ParserResult<Command> parseCommand(const std::string& line);

  /**
   * @brief Attempts to parse a raw value string into a typed protocol value.
   *
   * The parser uses the following heuristics, in order:
   * 1. If the string can be converted to a `double`, and contains `.` or scientific notation
   * (`e`/`E`), it is treated as a `double`.
   * 2. If the value is numeric and within `int` range, it is converted to `int`.
   * 3. If the value is quoted with `'`, `"`, or wrapped in `{}`, the delimiters are stripped and
   *    the result is returned as a `std::string`.
   * 4. Otherwise, the raw string is returned as-is.
   *
   * Examples:
   * - `"42"` → `int`
   * - `"3.14"` → `double`
   * - `"1e-3"` → `double`
   * - `"'hello'"`, `'"hello"'`, `"{hello}"` → `"hello"`
   * - `"raw_string"` → `"raw_string"`
   *
   * @param valueString The string representation of the value (right-hand side of an assignment).
   * @return A @ref irsol::types::protocol_value_t holding the parsed value.
   */
  static irsol::types::protocol_value_t parseValue(const std::string& valueString);
};

}  // namespace protocol
}  // namespace irsol
