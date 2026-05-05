#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "../src/benchmark_register.h"
#include "benchmark/benchmark_api.h"
#include "gtest/gtest.h"

namespace benchmark {
namespace internal {

namespace {

TEST(AddRangeTest, Simple) {
  std::vector<int> dst;
  AddRange(&dst, 1, 2, 2);

  const std::vector<int> expected = {1, 2};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, Simple64) {
  std::vector<int64_t> dst;
  AddRange(&dst, static_cast<int64_t>(1), static_cast<int64_t>(2), 2);

  const std::vector<int64_t> expected = {1, 2};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, Advanced) {
  std::vector<int> dst;
  AddRange(&dst, 5, 15, 2);

  const std::vector<int> expected = {5, 8, 15};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, Advanced64) {
  std::vector<int64_t> dst;
  AddRange(&dst, static_cast<int64_t>(5), static_cast<int64_t>(15), 2);

  const std::vector<int64_t> expected = {5, 8, 15};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, FullRange8) {
  std::vector<int8_t> dst;
  AddRange(&dst, int8_t{1}, std::numeric_limits<int8_t>::max(), 8);

  const std::vector<int8_t> expected = {
      int8_t{1}, int8_t{8}, int8_t{64}, int8_t{127}};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, FullRange64) {
  std::vector<int64_t> dst;
  AddRange(&dst, int64_t{1}, std::numeric_limits<int64_t>::max(), 1024);

  const std::vector<int64_t> expected = {
      1LL,
      1024LL,
      1048576LL,
      1073741824LL,
      1099511627776LL,
      1125899906842624LL,
      1152921504606846976LL,
      9223372036854775807LL};

  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, NegativeRanges) {
  std::vector<int> dst;
  AddRange(&dst, -8, 0, 2);

  const std::vector<int> expected = {-8, -4, -2, -1, 0};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, StrictlyNegative) {
  std::vector<int> dst;
  AddRange(&dst, -8, -1, 2);

  const std::vector<int> expected = {-8, -4, -2, -1};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, SymmetricNegativeRanges) {
  std::vector<int> dst;
  AddRange(&dst, -8, 8, 2);

  const std::vector<int> expected = {-8, -4, -2, -1, 0, 1, 2, 4, 8};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, SymmetricNegativeRangesOddMult) {
  std::vector<int> dst;
  AddRange(&dst, -30, 32, 5);

  const std::vector<int> expected = {-30, -25, -5, -1, 0, 1, 5, 25, 32};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, NegativeRangesAsymmetric) {
  std::vector<int> dst;
  AddRange(&dst, -3, 5, 2);

  const std::vector<int> expected = {-3, -2, -1, 0, 1, 2, 4, 5};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, NegativeRangesLargeStep) {
  // Always include -1, 0, 1 when crossing zero.
  std::vector<int> dst;
  AddRange(&dst, -8, 8, 10);

  const std::vector<int> expected = {-8, -1, 0, 1, 8};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, ZeroOnlyRange) {
  std::vector<int> dst;
  AddRange(&dst, 0, 0, 2);

  const std::vector<int> expected = {0};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, ZeroStartingRange) {
  std::vector<int> dst;
  AddRange(&dst, 0, 2, 2);

  const std::vector<int> expected = {0, 1, 2};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, NegativeRange64) {
  std::vector<int64_t> dst;
  AddRange<int64_t>(&dst, -4, 4, 2);

  const std::vector<int64_t> expected = {-4, -2, -1, 0, 1, 2, 4};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, NegativeRangePreservesExistingOrder) {
  // If elements already exist in the range, ensure we don't change
  // their ordering by adding negative values.
  std::vector<int64_t> dst = {1, 2, 3};
  AddRange<int64_t>(&dst, -2, 2, 2);

  const std::vector<int64_t> expected = {1, 2, 3, -2, -1, 0, 1, 2};
  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, FullNegativeRange64) {
  std::vector<int64_t> dst;
  const auto min = std::numeric_limits<int64_t>::min();
  const auto max = std::numeric_limits<int64_t>::max();

  AddRange(&dst, min, max, 1024);

  const std::vector<int64_t> expected = {
      min,
      -1152921504606846976LL,
      -1125899906842624LL,
      -1099511627776LL,
      -1073741824LL,
      -1048576LL,
      -1024LL,
      -1LL,
      0LL,
      1LL,
      1024LL,
      1048576LL,
      1073741824LL,
      1099511627776LL,
      1125899906842624LL,
      1152921504606846976LL,
      max};

  EXPECT_EQ(dst, expected);
}

TEST(AddRangeTest, Simple8) {
  std::vector<int8_t> dst;
  AddRange<int8_t>(&dst, int8_t{1}, int8_t{8}, int8_t{2});

  const std::vector<int8_t> expected = {
      int8_t{1}, int8_t{2}, int8_t{4}, int8_t{8}};
  EXPECT_EQ(dst, expected);
}

TEST(AddCustomContext, Simple) {
  std::map<std::string, std::string>*& global_context = GetGlobalContext();
  EXPECT_EQ(global_context, nullptr);

  AddCustomContext("foo", "bar");
  AddCustomContext("baz", "qux");

  ASSERT_NE(global_context, nullptr);

  std::map<std::string, std::string> expected;
  expected["foo"] = "bar";
  expected["baz"] = "qux";

  EXPECT_EQ(*global_context, expected);

  delete global_context;
  global_context = nullptr;
}

TEST(AddCustomContext, DuplicateKey) {
  std::map<std::string, std::string>*& global_context = GetGlobalContext();
  EXPECT_EQ(global_context, nullptr);

  AddCustomContext("foo", "bar");
  AddCustomContext("foo", "qux");

  ASSERT_NE(global_context, nullptr);

  std::map<std::string, std::string> expected;
  expected["foo"] = "bar";

  EXPECT_EQ(*global_context, expected);

  delete global_context;
  global_context = nullptr;
}

}  // namespace
}  // namespace internal
}  // namespace benchmark
