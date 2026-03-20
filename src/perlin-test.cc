#include "lightrand/perlin.h"
#include "lightrand/random.h"

#include <gtest/gtest.h>

namespace {

constexpr float EPSILON = 1e-6f;

// --- 1D Perlin Noise Tests ---

TEST(Perlin1DTest, ZeroAtIntegerCoordinates) {
  lightrand::generator gen(42);
  lightrand::perlin1d<> noise(gen);

  EXPECT_NEAR(noise.eval(0.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(1.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(100.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(-5.0f), 0.0f, EPSILON);
}

TEST(Perlin1DTest, WrapAroundAndPeriodicity) {
  lightrand::generator gen(12345);
  lightrand::perlin1d<256> noise(gen);

  // Verify that the noise repeats every TABLE_SIZE (256) units.
  // std::floor(-1.25) = -2, tx = 0.8. std::floor(254.75) = 254, tx = 0.8.
  EXPECT_FLOAT_EQ(noise.eval(0.5f), noise.eval(256.5f));
  EXPECT_FLOAT_EQ(noise.eval(-1.25f), noise.eval(254.75f));
}

TEST(Perlin1DTest, Determinism) {
  lightrand::generator gen1(999);
  lightrand::perlin1d<> noise1(gen1);

  lightrand::generator gen2(999);
  lightrand::perlin1d<> noise2(gen2);

  EXPECT_FLOAT_EQ(noise1.eval(1.23f), noise2.eval(1.23f));
  EXPECT_FLOAT_EQ(noise1.eval(-4.56f), noise2.eval(-4.56f));
}

// --- 2D Perlin Noise Tests ---

TEST(Perlin2DTest, ZeroAtIntegerCoordinates) {
  lightrand::generator gen(42);
  lightrand::perlin2d<> noise(gen);

  EXPECT_NEAR(noise.eval(0.0f, 0.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(1.0f, 2.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(-5.0f, 10.0f), 0.0f, EPSILON);
}

TEST(Perlin2DTest, WrapAroundAndPeriodicity) {
  lightrand::generator gen(12345);
  lightrand::perlin2d<256> noise(gen);

  EXPECT_FLOAT_EQ(noise.eval(0.5f, 0.25f), noise.eval(256.5f, 256.25f));
  EXPECT_FLOAT_EQ(noise.eval(-1.25f, 4.5f), noise.eval(254.75f, 260.5f));
}

TEST(Perlin2DTest, Determinism) {
  lightrand::generator gen1(999);
  lightrand::perlin2d<> noise1(gen1);

  lightrand::generator gen2(999);
  lightrand::perlin2d<> noise2(gen2);

  EXPECT_FLOAT_EQ(noise1.eval(1.23f, 4.56f), noise2.eval(1.23f, 4.56f));
  EXPECT_FLOAT_EQ(noise1.eval(-7.89f, 0.12f), noise2.eval(-7.89f, 0.12f));
}

// --- 3D Perlin Noise Tests ---

TEST(Perlin3DTest, ZeroAtIntegerCoordinates) {
  lightrand::generator gen(42);
  lightrand::perlin3d<> noise(gen);

  EXPECT_NEAR(noise.eval(0.0f, 0.0f, 0.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(1.0f, 2.0f, 3.0f), 0.0f, EPSILON);
  EXPECT_NEAR(noise.eval(-5.0f, 10.0f, -15.0f), 0.0f, EPSILON);
}

TEST(Perlin3DTest, WrapAroundAndPeriodicity) {
  lightrand::generator gen(12345);
  lightrand::perlin3d<256> noise(gen);

  EXPECT_FLOAT_EQ(noise.eval(0.5f, 0.25f, 0.125f),
                  noise.eval(256.5f, 256.25f, 256.125f));
  EXPECT_FLOAT_EQ(noise.eval(-1.25f, 4.5f, -0.75f),
                  noise.eval(254.75f, 260.5f, 255.25f));
}

TEST(Perlin3DTest, Determinism) {
  lightrand::generator gen1(999);
  lightrand::perlin3d<> noise1(gen1);

  lightrand::generator gen2(999);
  lightrand::perlin3d<> noise2(gen2);

  EXPECT_FLOAT_EQ(noise1.eval(1.23f, 4.56f, 7.89f),
                  noise2.eval(1.23f, 4.56f, 7.89f));
  EXPECT_FLOAT_EQ(noise1.eval(-0.12f, 3.45f, -6.78f),
                  noise2.eval(-0.12f, 3.45f, -6.78f));
}

} // namespace
