#include "lightrand/random.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <limits>

namespace {

// --- PRNG Engine Tests ---

TEST(SplitMix64Test, MinMax) {
  EXPECT_EQ(lightrand::splitmix64::min(), 0);
  EXPECT_EQ(lightrand::splitmix64::max(),
            std::numeric_limits<std::uint64_t>::max());
}

TEST(Xoshiro256StarStarTest, MinMax) {
  EXPECT_EQ(lightrand::xoshiro256starstar::min(), 0);
  EXPECT_EQ(lightrand::xoshiro256starstar::max(),
            std::numeric_limits<std::uint64_t>::max());
}

TEST(SplitMix64Test, Determinism) {
  lightrand::splitmix64 gen1(42);
  lightrand::splitmix64 gen2(42);

  for (int i = 0; i < 100; ++i)
    EXPECT_EQ(gen1(), gen2());
}

TEST(Xoshiro256StarStarTest, Determinism) {
  lightrand::xoshiro256starstar gen1(1337);
  lightrand::xoshiro256starstar gen2(1337);

  for (int i = 0; i < 100; ++i)
    EXPECT_EQ(gen1(), gen2());
}

// --- Generator & Distribution Tests ---

TEST(GeneratorTest, Determinism) {
  lightrand::thread_urbg.seed(12345);
  lightrand::generator gen1(lightrand::thread_urbg);
  lightrand::engine eng1(12345);
  lightrand::generator gen2(eng1);

  for (int i = 0; i < 100; ++i) {
    auto v1 = gen1.uniform<int>(10, 20);
    auto v2 = gen2.uniform<int>(10, 20);
    EXPECT_EQ(v1, v2);
  }
}

TEST(GeneratorTest, DefaultConstructor) {
  // Instantiates with default seed (std::nullopt) and default engine
  // (thread_urbg)
  lightrand::generator gen;

  int val = gen.uniform<int>(10, 20);
  EXPECT_GE(val, 10);
  EXPECT_LE(val, 20);
}

TEST(GeneratorTest, UniformIntLimits) {
  lightrand::generator gen;
  int min_val = 10;
  int max_val = 20;

  for (int i = 0; i < 1000; ++i) {
    int val = gen.uniform<int>(min_val, max_val);
    EXPECT_GE(val, min_val);
    EXPECT_LE(val, max_val);
  }
}

TEST(GeneratorTest, UniformIntDefaultHi) {
  lightrand::generator gen;
  int min_val = std::numeric_limits<int>::max() - 100;

  for (int i = 0; i < 1000; ++i) {
    int val = gen.uniform<int>(min_val);
    EXPECT_GE(val, min_val);
    EXPECT_LE(val, std::numeric_limits<int>::max());
  }
}

TEST(GeneratorTest, UniformFloatDefaultRange) {
  lightrand::generator gen;

  for (int i = 0; i < 1000; ++i) {
    auto val_d = gen.uniform<double>();
    EXPECT_GE(val_d, 0.0);
    EXPECT_LT(val_d, 1.0);

    auto val_f = gen.uniform<float>();
    EXPECT_GE(val_f, 0.0f);
    EXPECT_LT(val_f, 1.0f);
  }
}

TEST(GeneratorTest, UniformFloatCustomRange) {
  lightrand::generator gen;
  double min_val = 1.5;
  double max_val = 5.5;

  for (int i = 0; i < 1000; ++i) {
    auto val = gen.uniform<double>(min_val, max_val);
    EXPECT_GE(val, min_val);
    EXPECT_LT(val, max_val);
  }
}

TEST(GeneratorTest, UniformFloatDefaultHi) {
  lightrand::generator gen;
  double min_val = 0.5;

  for (int i = 0; i < 1000; ++i) {
    auto val = gen.uniform<double>(min_val);
    EXPECT_GE(val, min_val);
    EXPECT_LE(val, 1.0);
  }
}

TEST(GeneratorTest, NormalDistributionStats) {
  lightrand::generator gen;

  double sum = 0.0;
  double sq_sum = 0.0;
  const int n = 100000;
  const double expected_mean = 5.0;
  const double expected_stddev = 2.0;

  for (int i = 0; i < n; ++i) {
    auto val = gen.normal<double>(expected_mean, expected_stddev);
    sum += val;
    sq_sum += val * val;
  }

  double mean = sum / n;
  double variance = (sq_sum / n) - (mean * mean);
  double stddev = std::sqrt(variance);

  // Verify with a small tolerance margin due to the nature of randomness
  EXPECT_NEAR(mean, expected_mean, 0.05);
  EXPECT_NEAR(stddev, expected_stddev, 0.05);
}

TEST(GeneratorTest, NormalDefaultStddev) {
  lightrand::generator gen;

  double sum = 0.0;
  double sq_sum = 0.0;
  const int n = 1000000;
  const double expected_mean = 5.0;
  const double expected_stddev = 1.0;

  for (int i = 0; i < n; ++i) {
    // Pass only the mean, leaving standard deviation defaulted to 1.0
    auto val = gen.normal<double>(expected_mean);
    sum += val;
    sq_sum += val * val;
  }

  double mean = sum / n;
  double variance = (sq_sum / n) - (mean * mean);
  double stddev = std::sqrt(variance);

  EXPECT_NEAR(mean, expected_mean, 0.05);
  EXPECT_NEAR(stddev, expected_stddev, 0.05);
}

} // namespace
