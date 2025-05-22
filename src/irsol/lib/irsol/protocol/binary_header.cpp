#include "irsol/protocol/binary_header.hpp"

#include "irsol/protocol/types.hpp"

#include <regex>

namespace irsol {
namespace protocol {

BinaryHeader
BinaryHeaderParser::parse(const std::string& header)
{
  BinaryHeader bh;
  std::smatch  m;
  // Type and dimensions
  static const std::regex reType(R"(^([su f]\d+))");
  if(std::regex_search(header, m, reType)) {
    bh.type = m[1];
    std::stringstream ss(m[2]);
    std::string       dim;
    while(std::getline(ss, dim, ',')) {
      bh.dimensions.push_back(std::stoi(dim));
    }
  }
  // Metadata key=value pairs
  static const std::regex reKV(R"((\w+)=([^ \r\n]+))");
  auto                    begin = std::sregex_iterator(header.begin(), header.end(), reKV);
  auto                    end   = std::sregex_iterator();
  for(auto it = begin; it != end; ++it) {
    const auto& m2     = *it;
    bh.metadata[m2[1]] = m2[2];
  }
  return bh;
}
}
// namespace protocol
}  // namespace irsol