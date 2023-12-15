#include <sstream>
#include <string_view>

#include "test_header.hh"

#include "common/common.hh"

using namespace leech;

TEST(Instruction, simple) {
  // Assign
  auto inst = Instruction(Opcodes::BINARY_ADD, 15);
  std::ostringstream ss;
  std::string_view answ("\x11\xF");
  // Act
  inst.serialize(ss);
  // Assert
  EXPECT_EQ(ss.str(), answ);
}

TEST(Instruction, bad) {
  EXPECT_THROW(Instruction(Opcodes::UNKNOWN, 228), std::invalid_argument);
}

#include "test_footer.hh"
