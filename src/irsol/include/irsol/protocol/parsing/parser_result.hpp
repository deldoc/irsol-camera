#pragma once

#include "irsol/protocol/message/variants.hpp"
#include "irsol/traits.hpp"

#include <string>
#include <variant>

namespace irsol {
namespace protocol {
namespace internal {

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
class ParserResult
{
public:
  using result_type = T;
  using error_type  = std::string;

  ParserResult(result_type&& message): _messageOrError(std::move(message)) {}
  ParserResult(error_type&& error): _messageOrError(std::move(error)) {}

  // Delete copy constructor and move assignment operator and copy assignment operator
  ParserResult(const ParserResult&) = delete;
  ParserResult& operator=(ParserResult&&) noexcept = delete;
  ParserResult& operator=(const ParserResult&) = delete;

  explicit operator bool() const
  {
    return _messageOrError.index() == 0;
  }
  bool isMessage() const
  {
    return this->operator bool();
  }
  bool isError() const
  {
    return !this->operator bool();
  }
  result_type getMessage() const
  {
    return std::get<result_type>(_messageOrError);
  }
  error_type getError() const
  {
    return std::get<error_type>(_messageOrError);
  }

private:
  std::variant<result_type, error_type> _messageOrError;
};
}  // namespace internal
}  // namespace protocol
}  // namespace irsol