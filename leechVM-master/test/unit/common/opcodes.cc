#include "common/opcodes.hh"
#include "test_header.hh"
#include <string>
#include <string_view>

using namespace leech;

TEST(StringToOpcode, string) {
  std::string pop = "POP_TOP";
  std::string err = "HAHA";

  auto opcode = OpcodeConv::fromName(pop);
  auto opcodeErr = OpcodeConv::fromName(err);

  EXPECT_EQ(opcode.value(), Opcodes::POP_TOP);
  EXPECT_THROW(opcodeErr.value(), std::bad_optional_access);
}

TEST(StringToOpcode, string_view) {
  std::string_view correct = "BINARY_OR";
  std::string_view err = "HAHA";

  auto opcode = OpcodeConv::fromName(correct);
  auto opcodeErr = OpcodeConv::fromName(err);

  EXPECT_EQ(opcode.value(), Opcodes::BINARY_OR);
  EXPECT_THROW(opcodeErr.value(), std::bad_optional_access);
}

TEST(StringToOpcode, const_char_ptr) {
  const char *correct = "GEN_START";
  const char *err = "HAHA";

  auto opcode = OpcodeConv::fromName(correct);
  auto opcodeErr = OpcodeConv::fromName(err);

  EXPECT_EQ(opcode.value(), Opcodes::GEN_START);
  EXPECT_THROW(opcodeErr.value(), std::bad_optional_access);
}

TEST(OpcodeToString, basic) {
  Opcodes correct = Opcodes::CALL_FUNCTION_KW;
  Opcodes err = Opcodes::UNKNOWN;

  auto name = OpcodeConv::toName(correct);
  auto nameErr = OpcodeConv::toName(err);

  EXPECT_EQ(name.value(), "CALL_FUNCTION_KW");
  EXPECT_THROW(nameErr.value(), std::bad_optional_access);
}

#include "test_footer.hh"
