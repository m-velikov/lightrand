#include "lightrand/random.h"

#include <gtest/gtest.h>

namespace {

// --- PRNG Engine Tests ---

TEST(SplitMix64Test, Determinism) {
  lightrand::splitmix64 gen1(42);
  lightrand::splitmix64 gen2(42);

  for (int i = 0; i < 100; ++i)
    EXPECT_EQ(gen1(), gen2());
}

TEST(SplitMix64Test, DefaultSeed) {
  lightrand::splitmix64 gen1;
  lightrand::splitmix64 gen2(0xcafebabe);

  for (int i = 0; i < 100; ++i)
    EXPECT_EQ(gen1(), gen2());
}

TEST(Xoshiro256StarStarTest, Determinism) {
  lightrand::xoshiro256starstar gen1(1337);
  lightrand::xoshiro256starstar gen2(1337);

  for (int i = 0; i < 100; ++i)
    EXPECT_EQ(gen1(), gen2());
}

TEST(Xoshiro256StarStarTest, DefaultSeed) {
  lightrand::xoshiro256starstar gen1;
  lightrand::xoshiro256starstar gen2(0xc0ffeef00d);

  for (int i = 0; i < 100; ++i)
    EXPECT_EQ(gen1(), gen2());
}

// --- Generator & Distribution Tests ---

TEST(GeneratorTest, Determinism) {
  lightrand::generator gen1(12345, lightrand::global_urbg);
  lightrand::generator gen2(12345, lightrand::thread_urbg);
  lightrand::engine eng(12345);
  lightrand::generator gen3(12345, eng);

  for (int i = 0; i < 100; ++i) {
    auto v1 = gen1.uniform<int>(10, 20);
    auto v2 = gen2.uniform<int>(10, 20);
    auto v3 = gen3.uniform<int>(10, 20);
    EXPECT_EQ(v1, v2);
    EXPECT_EQ(v1, v3);
  }
}

TEST(GeneratorTest, DefaultConstructor) {
  // Instantiates with default seed (0xc0ffeef00d) and default engine
  // (global_urbg)
  lightrand::generator gen;

  int val = gen.uniform<int>(10, 20);
  EXPECT_GE(val, 10);
  EXPECT_LE(val, 20);
}

TEST(GeneratorTest, UniformIntLimits) {
  lightrand::generator gen(12345);
  int min_val = 10;
  int max_val = 20;

  for (int i = 0; i < 1000; ++i) {
    int val = gen.uniform<int>(min_val, max_val);
    EXPECT_GE(val, min_val);
    EXPECT_LE(val, max_val);
  }
}

TEST(GeneratorTest, UniformIntDefaultHi) {
  lightrand::generator gen(12345);
  int min_val = std::numeric_limits<int>::max() - 100;

  for (int i = 0; i < 1000; ++i) {
    int val = gen.uniform<int>(min_val);
    EXPECT_GE(val, min_val);
    EXPECT_LE(val, std::numeric_limits<int>::max());
  }
}

TEST(GeneratorTest, UniformFloatDefaultRange) {
  lightrand::generator gen(12345);

  for (int i = 0; i < 1000; ++i) {
    double val_d = gen.uniform<double>();
    EXPECT_GE(val_d, 0.0);
    EXPECT_LT(val_d, 1.0);

    float val_f = gen.uniform<float>();
    EXPECT_GE(val_f, 0.0f);
    EXPECT_LT(val_f, 1.0f);
  }
}

TEST(GeneratorTest, UniformFloatCustomRange) {
  lightrand::generator gen(12345);
  double min_val = 1.5;
  double max_val = 5.5;

  for (int i = 0; i < 1000; ++i) {
    double val = gen.uniform<double>(min_val, max_val);
    EXPECT_GE(val, min_val);
    EXPECT_LT(val, max_val);
  }
}

TEST(GeneratorTest, UniformFloatDefaultHi) {
  lightrand::generator gen(12345);
  double min_val = 0.5;

  for (int i = 0; i < 1000; ++i) {
    double val = gen.uniform<double>(min_val);
    EXPECT_GE(val, min_val);
    EXPECT_LE(val, 1.0);
  }
}

TEST(GeneratorTest, NormalDistributionStats) {
  lightrand::generator gen(12345);

  double sum = 0.0;
  double sq_sum = 0.0;
  const int n = 100000;
  const double expected_mean = 5.0;
  const double expected_stddev = 2.0;

  for (int i = 0; i < n; ++i) {
    double val = gen.normal<double>(expected_mean, expected_stddev);
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
  lightrand::generator gen(12345);

  double sum = 0.0;
  double sq_sum = 0.0;
  const int n = 1000000;
  const double expected_mean = 5.0;
  const double expected_stddev = 1.0;

  for (int i = 0; i < n; ++i) {
    // Pass only the mean, leaving standard deviation defaulted to 1.0
    double val = gen.normal<double>(expected_mean);
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
