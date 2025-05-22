#pragma once
#include "irsol/protocol/message.hpp"
#include "irsol/protocol/types.hpp"

#include <string>

namespace irsol {
namespace protocol {

class Serializer
{
public:
  static std::string serialize(const OutMessage& msg);

private:
  static std::string serializeValue(const internal::value_t& value);
};

}  // namespace protocol
}  // namespace irsol
