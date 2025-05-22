#pragma once

#include "irsol/protocol/types.hpp"

#include <optional>
#include <string>
#include <variant>

namespace irsol {
namespace protocol {

struct Assignment
{
  std::string       identifier;
  internal::value_t value;

  std::string toString() const;

  bool hasInt() const;
  bool hasDouble() const;
  bool hasString() const;
};

struct Inquiry
{
  std::string identifier;

  std::string toString() const;
};

struct Command
{
  std::string identifier;

  std::string toString() const;
};

struct Status
{
  std::string                identifier;
  std::optional<std::string> body{};

  std::string toString() const;

  bool hasBody() const;
};

struct Error
{
  std::string identifier;
  std::string description;

  std::string toString() const;
};

using InMessage  = std::variant<Assignment, Inquiry, Command>;
using OutMessage = std::variant<Status, Error>;

}  // namespace protocol
}  // namespace irsol
