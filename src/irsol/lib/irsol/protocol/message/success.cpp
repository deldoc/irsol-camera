
#include "irsol/protocol/message/success.hpp"

#include "irsol/protocol/utils.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
Success::Success(
  const std::string&                            identifier,
  InMessageKind                                 source,
  std::optional<irsol::types::protocol_value_t> body)
  : identifier(utils::validateIdentifier(identifier)), source(source), body(body)
{}

std::string
Success::toString() const
{
  std::ostringstream oss;
  oss << "Success{"
      << "identifier: '" << identifier << "', source: " << InMessageKindToString(source);
  if(hasBody()) {
    oss << ", body: ";
    if(hasInt()) {
      oss << "<int> " << std::get<int>(*body);
    } else if(hasDouble()) {
      oss << "<double> " << std::get<double>(*body);
    } else if(hasString()) {
      oss << "<string> \"" << std::get<std::string>(*body) << "\"";
    }
  }
  oss << "}";
  return oss.str();
}

bool
Success::hasBody() const
{
  return body.has_value();
}
bool
Success::hasInt() const
{
  return hasBody() && std::holds_alternative<int>(*body);
}
bool
Success::hasDouble() const
{
  return hasBody() && std::holds_alternative<double>(*body);
}
bool
Success::hasString() const
{
  return hasBody() && std::holds_alternative<std::string>(*body);
}

}  // namespace protocol
}  // namespace irsol