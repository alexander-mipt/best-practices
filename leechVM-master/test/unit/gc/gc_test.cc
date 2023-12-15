#include "test_header.hh"
#include "gc/gc.hh"
#include <stdexcept>

using namespace leech;

TEST(MemManager, DoubleMMap)
{
  gc::MemoryManager mman;


  EXPECT_THROW(gc::MemoryManager(), std::logic_error);
}

#include "test_footer.hh"
