#include "irsol/protocol/serializer.hpp"

#include "irsol/protocol/types.hpp"
#include "irsol/protocol/utils.hpp"

#include <variant>

namespace irsol {
namespace protocol {

std::string
Serializer::serialize(const OutMessage& msg)
{

  return std::visit(
    [](auto&& msg) -> std::string {
      using T = std::decay_t<decltype(msg)>;
      return serialize<T>(msg);
    },
    msg);
}

std::string
Serializer::serializeValue(const internal::value_t& value)
{
  return std::visit(
    [](auto&& val) -> std::string {
      using T = std::decay_t<decltype(val)>;
      return serializeValue<T>(val);
    },
    value);
}
}  // namespace protocol
}  // namespace irsol