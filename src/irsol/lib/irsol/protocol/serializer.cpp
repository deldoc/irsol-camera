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
      if constexpr(std::is_same_v<T, Status>) {
        return msg.identifier + ";";
      } else if constexpr(std::is_same_v<T, BinaryDataBuffer>) {
        return "todo";
      } else if constexpr(std::is_same_v<T, ImageBinaryData>) {
        return "todo";
      } else if constexpr(std::is_same_v<T, ColorImageBinaryData>) {
        return "todo";
      } else if constexpr(std::is_same_v<T, Error>) {
        return msg.identifier + ": ERROR: " + msg.description;
      } else {
        static_assert(sizeof(T) == 0, "serialize: unsupported type");
        return "";
      }
    },
    msg);
}

std::string
Serializer::serializeValue(const internal::value_t& value)
{
  return std::visit(
    [](auto&& val) -> std::string {
      using T = std::decay_t<decltype(val)>;
      if constexpr(std::is_same_v<T, std::string>) {
        return "{" + val + "}";  // wrap string in braces as per protocol specification
      } else {
        return std::to_string(val);  // int and double
      }
    },
    value);
}
}  // namespace protocol
}  // namespace irsol