#include "test_header.hh"

TEST(example, test)
{
  int a = 30, b = 12;

  ASSERT_EQ(a + b, 42);
}


#include "test_footer.hh"
