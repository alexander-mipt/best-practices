#ifndef __INCLUDE_COMMON_COMMON_HH__
#define __INCLUDE_COMMON_COMMON_HH__

#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "opcodes.hh"

namespace leech {

using FuncAddr = std::uint64_t;
enum class ValueType : std::uint8_t {
  Unknown,
  Integer,
  Float,
  String,
  Tuple,
  Class,
  None
};

enum class CmpOp : std::uint8_t {
  LE = 0,
  LEQ,
  EQ,
  NEQ,
  GR,
  GREQ,
};

template <typename T> constexpr auto toUnderlying(T enumVal) {
  return static_cast<std::underlying_type_t<T>>(enumVal);
}

using Integer = std::int64_t;
using Float = double;

template <typename T>
constexpr bool NumberLeech_v =
    std::is_same_v<T, Integer> || std::is_same_v<T, Float>;

template <typename T>
constexpr bool Number_v = NumberLeech_v<T> || std::is_integral_v<T>;

template <typename T> inline constexpr ValueType typeToValueType() {
  static_assert(NumberLeech_v<T>);
  if constexpr (std::is_same_v<T, Integer>)
    return ValueType::Integer;
  return ValueType::Float;
}

template <typename T> void serializeNum(std::ostream &ost, T val) {
  static_assert(Number_v<T>);
  ost.write(reinterpret_cast<char *>(&val), sizeof(val));
}

inline void serializeString(std::ostream &ost, std::string_view sv) {
  auto svLen = sv.size();
  serializeNum(ost, svLen);
  if (svLen > 0)
    ost.write(sv.data(), static_cast<std::streamsize>(
                             svLen * sizeof(std::string_view::value_type)));
}

template <typename T> T deserializeNum(std::istream &ist) {
  static_assert(Number_v<T>);
  T num{};
  if (!ist.read(reinterpret_cast<char *>(&num), sizeof(num)))
    throw std::runtime_error{"Can't read number from istream"};
  return num;
}

inline std::string deserializeString(std::istream &ist, std::size_t size) {
  std::string res{};
  res.resize(size);
  if (!ist.read(res.data(), static_cast<std::streamsize>(size)))
    throw std::runtime_error{"Can't read string from istream"};
  return res;
}

struct ISerializable {
  virtual void serialize(std::ostream &ost) const = 0;
  virtual ~ISerializable() = default;
};

using ArgType = std::uint8_t;

constexpr std::size_t kArgSize = sizeof(ArgType);
constexpr std::size_t kInstSize =
    sizeof(std::underlying_type_t<Opcodes>) + kArgSize;

struct State;

class Instruction final : public ISerializable {
  using Callback = void (*)(const Instruction &, State &);
  Opcodes opcode_{};
  ArgType arg_{};
  Callback callback{};

  static const std::unordered_map<Opcodes, Callback> opcToCallback;

public:
  explicit Instruction(Opcodes opcode, ArgType arg = 0)
      : opcode_(opcode), arg_(arg) {
    if (Opcodes::UNKNOWN == opcode_)
      throw std::invalid_argument(
          "Trying to create Instruction with UNKNOWN opcode");

    if (auto callback_it = opcToCallback.find(opcode_);
        callback_it != opcToCallback.end())
      callback = callback_it->second;
    else
      throw std::runtime_error{
          "Unknown inst opcode: " +
          std::to_string(static_cast<unsigned>(toUnderlying(opcode_)))};
  }

  explicit Instruction(std::underlying_type_t<Opcodes> opcode, ArgType arg = 0)
      : Instruction(static_cast<Opcodes>(opcode), arg) {}

  Instruction() = default;

  void serialize(std::ostream &ost) const override {
    serializeNum(ost, toUnderlying(opcode_));
    serializeNum(ost, arg_);
  }

  [[nodiscard]] auto getOpcode() const { return opcode_; }
  [[nodiscard]] auto getArg() const { return arg_; }
  void setArg(ArgType arg) { arg_ = arg; }

  void execute(State &state) const { callback(*this, state); }

  static auto deserialize(std::istream &ist) {
    auto opcodeVal = deserializeNum<std::underlying_type_t<Opcodes>>(ist);
    auto opcode = static_cast<Opcodes>(opcodeVal);
    auto arg = deserializeNum<ArgType>(ist);
    return Instruction{opcode, arg};
  }
};

} // namespace leech

#endif // __INCLUDE_COMMON_COMMON_HH__
