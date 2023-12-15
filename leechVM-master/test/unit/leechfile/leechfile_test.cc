#include <sstream>
#include <string_view>

#include "leechfile/leechfile.hh"
#include "test_header.hh"

using namespace leech;

TEST(Serialize, Empty) {
  // Assign
  LeechFile cf{};
  std::ostringstream oss{};
  std::string_view ans{"theLEECH"
                       "\x0\x0\x0\x0\x0\x0\x0\x0"
                       "\x0\x0\x0\x0\x0\x0\x0\x0",
                       24};

  // Act
  cf.serialize(oss);

  // Assert
  EXPECT_EQ(oss.str(), ans);
}

#include "test_footer.hh"
