#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/alloc.h"
#include "include/allocator.h"

using namespace ministl;

TEST(test1, allocator_test) {
  allocator<std::string> alloc;
  auto str_ve = alloc.allocate(4);
  auto p = str_ve;
  alloc.construct(p++, "construct");
  EXPECT_EQ(str_ve[0], "construct");
  alloc.destroy(--p);
  alloc.deallocate(str_ve);
}
