#pragma once
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace irsol {
namespace protocol {

struct BinaryHeader
{
  std::string                                  type;        // e.g., "u16"
  std::vector<int>                             dimensions;  // e.g., {560, 1280}
  std::unordered_map<std::string, std::string> metadata;
};

class BinaryHeaderParser
{
public:
  static BinaryHeader parse(const std::string& header);
};

}  // namespace protocol
}  // namespace irsol
