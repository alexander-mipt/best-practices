#include <sstream>
#include <string_view>

#include "leechobj/leechobj.hh"
#include "test_header.hh"

using namespace leech;

TEST(Serialize, Int) {
  // Assign
  Integer val = 0xE4;
  NumberObj<Integer> integer(val);
  std::ostringstream ss;
  std::string_view answ("\x1\x8\x0\x0\x0\x0\x0\x0\x0\xE4\x0\x0\x0\x0\x0\x0\x0",
                        1 + sizeof(std::size_t) + sizeof(Integer));
  // Act
  integer.serialize(ss);

  // Assert
  EXPECT_EQ(ss.str(), answ);
}

TEST(Serialize, Float) {
  // Assign
  auto val = static_cast<Float>(0xFFFFFFFF88E368F1);
  NumberObj<decltype(val)> flt(val);
  std::ostringstream ss;
  std::string_view answ(
      "\x2\x8\x0\x0\x0\x0\x0\x0\x0\x6D\x1C\xF1\xFF\xFF\xFF\xEF\x43",
      1 + sizeof(std::size_t) + sizeof(val));
  // Act
  flt.serialize(ss);

  // Assert
  EXPECT_EQ(ss.str(), answ);
}

TEST(Serialize, String) {
  // Assign
  std::string val = "Hello world!!!";
  StringObj str(val);
  std::ostringstream ss;
  std::string_view answ("\x3\xE\x0\x0\x0\x0\x0\x0\x0"
                        "Hello world!!!",
                        1 + sizeof(std::size_t) + val.size());
  // Act
  str.serialize(ss);

  // Assert
  EXPECT_EQ(ss.str(), answ);
}

TEST(Serialize, Tuple) {
  // Assign
  Tuple tup;
  Tuple tup2;
  std::string_view str("HELLO!");
  tup2.emplace_back(new NumberObj<Integer>(-1));
  tup.emplace_back(new TupleObj(std::move(tup2)));
  tup.emplace_back(new StringObj(str));
  TupleObj tuple(std::move(tup));

  std::ostringstream ss;
  std::string_view answ("\x4\x2\x0\x0\x0\x0\x0\x0\x0"
                        "\x4\x1\x0\x0\x0\x0\x0\x0\x0"
                        "\x1\x8\x0\x0\x0\x0\x0\x0\x0"
                        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                        "\x3\x6\x0\x0\x0\x0\x0\x0\x0"
                        "HELLO!",
                        (1 + sizeof(std::size_t)) * 4 + sizeof(Integer) +
                            str.size());
  // Act
  tuple.serialize(ss);

  // Assert
  EXPECT_EQ(ss.str(), answ);
}

TEST(Serialize, None) {
  // Assign
  NoneObj none;
  std::ostringstream ss;
  std::string_view answ("\x5");
  // Act
  none.serialize(ss);
  // Assert
  EXPECT_EQ(ss.str(), answ);
}

#include "test_footer.hh"
