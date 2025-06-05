
/**
 * @file irsol/protocol/parsing/parser_result.hpp
 * @brief Encapsulates the result of a protocol parsing operation.
 * @ingroup Protocol
 *
 * @tparam T The expected message type, constrained to be one of the @ref irsol::protocol::InMessage
 * variant types.
 *
 * This class holds either a successfully parsed message of type `T` or a parsing error message
 * (`std::string`). It provides convenient accessors and status-checking methods to determine
 * whether the result was successful or an error.
 */
#pragma once

#include "irsol/macros.hpp"
#include "irsol/protocol/message/variants.hpp"
#include "irsol/traits.hpp"

#include <sstream>
#include <string>
#include <variant>

namespace irsol {
namespace protocol {
namespace internal {
/**
 * @brief Wrapper for the result of a protocol parsing attempt.
 *
 * @tparam T The message type expected from parsing, constrained to be a variant member of @ref
 * irsol::protocol::InMessage.
 *
 * @ingroup Protocol
 *
 * This class represents the outcome of trying to parse a protocol input line into a specific
 * message type. It holds either a successfully parsed message of type `T` or an error message
 * describing why parsing failed.
 *
 * @see irsol::protocol::Parser
 *
 * The interface allows:
 * - Checking if parsing was successful (`operator bool()`, `isMessage()`)
 * - Retrieving the parsed message (`getMessage()`)
 * - Retrieving the error message if parsing failed (`getError()`)
 * - Getting a human-readable string describing the result (`toString()`)
 *
 * This class disables copying and move assignment to prevent accidental duplication of results.
 *
 * Example:
 * ```cpp
 * ParserResult<Assignment> result = Parser::parseAssignment("foo=42");
 * if (result) {
 *     auto msg = result.getMessage();
 *     // Process msg...
 * } else {
 *     std::cerr << "Parse error: " << result.getError() << std::endl;
 * }
 * ```
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
class ParserResult
{
public:
  using result_type  = T;
  using error_type   = std::string;
  using message_type = std::variant<result_type, error_type>;

  /**
   * @brief Constructs a successful parser result.
   * @param message A valid parsed message.
   */
  ParserResult(result_type&& message): _messageOrError(std::move(message)) {}
  /**
   * @brief Constructs a failed parser result with an error string.
   * @param error A descriptive error message.
   */
  ParserResult(error_type&& error): _messageOrError(std::move(error)) {}

  // Delete copy constructor and move assignment operator and copy assignment operator
  ParserResult(const ParserResult&) = delete;
  ParserResult& operator=(ParserResult&&) noexcept = delete;
  ParserResult& operator=(const ParserResult&) = delete;
  /**
   * @brief Checks whether this result contains a valid message.
   * @return true if a message is stored; false if it's an error.
   */
  explicit operator bool() const
  {
    return isMessage();
  }
  /**
   * @brief Returns true if the result contains a parsed message.
   */
  bool isMessage() const
  {
    return std::holds_alternative<result_type>(_messageOrError);
  }
  /**
   * @brief Returns true if the result contains an error message.
   */
  bool isError() const
  {
    return std::holds_alternative<error_type>(_messageOrError);
  }

  /**
   * @brief Returns the contained message.
   * @throws std::bad_variant_access if no message is stored.
   */
  result_type getMessage() const
  {
    return std::get<result_type>(_messageOrError);
  }
  /**
   * @brief Returns the contained error string.
   * @throws std::bad_variant_access if no error is stored.
   */
  error_type getError() const
  {
    return std::get<error_type>(_messageOrError);
  }

  /**
   * @brief Returns a human-readable string representation of the result.
   *
   * @return A human-readable string representation of the result.
   */
  std::string toString() const
  {
    std::string message_string;
    std::string message_type;
    if(isMessage()) {
      message_string = getMessage().toString();
      message_type   = "Message";
    } else if(isError()) {
      message_string = getError();
      message_type   = "Error";
    } else {
      IRSOL_ASSERT_FATAL("Both 'isMessage()' and 'isError()' returned false");
    }

    std::stringstream ss;
    ss << "ParserResult<" << message_type << ">('" << message_string << "')";
    return ss.str();
  }

private:
  message_type _messageOrError;
};
}  // namespace internal
}  // namespace protocol
}  // namespace irsol