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

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
class ParserResult
{
public:
  using result_type  = T;
  using error_type   = std::string;
  using message_type = std::variant<result_type, error_type>;

  ParserResult(result_type&& message): _messageOrError(std::move(message)) {}
  ParserResult(error_type&& error): _messageOrError(std::move(error)) {}

  // Delete copy constructor and move assignment operator and copy assignment operator
  ParserResult(const ParserResult&) = delete;
  ParserResult& operator=(ParserResult&&) noexcept = delete;
  ParserResult& operator=(const ParserResult&) = delete;

  explicit operator bool() const
  {
    return isMessage();
  }
  bool isMessage() const
  {
    return std::holds_alternative<result_type>(_messageOrError);
  }
  bool isError() const
  {
    return std::holds_alternative<error_type>(_messageOrError);
  }
  result_type getMessage() const
  {
    return std::get<result_type>(_messageOrError);
  }
  error_type getError() const
  {
    return std::get<error_type>(_messageOrError);
  }

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
      IRSOL_UNREACHABLE("Invalid ParserResult state");
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