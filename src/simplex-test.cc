#include "lightrand/random.h"
#include "lightrand/simplex.h"

#include <gtest/gtest.h>

namespace {

constexpr float EPSILON = 1e-6f;

// --- 2D Simplex Noise Tests ---

TEST(Simplex2DTest, OriginIsZero) {
  lightrand::generator gen(42);
  lightrand::simplex2d<> noise(gen);

  // Simplex distance vectors yield 0.0 exactly at the unskewed origin
  EXPECT_NEAR(noise.eval(0.0f, 0.0f), 0.0f, EPSILON);
}

TEST(Simplex2DTest, Determinism) {
  lightrand::generator gen1(999);
  lightrand::simplex2d<> noise1(gen1);

  lightrand::generator gen2(999);
  lightrand::simplex2d<> noise2(gen2);

  EXPECT_FLOAT_EQ(noise1.eval(1.23f, 4.56f), noise2.eval(1.23f, 4.56f));
  EXPECT_FLOAT_EQ(noise1.eval(-7.89f, 0.12f), noise2.eval(-7.89f, 0.12f));
}

TEST(Simplex2DTest, Bounds) {
  lightrand::generator gen(12345);
  lightrand::simplex2d<> noise(gen);

  // Verify that outputs are generally within the [-1.0, 1.0] range
  for (float x = -10.0f; x < 10.0f; x += 0.5f) {
    for (float y = -10.0f; y < 10.0f; y += 0.5f) {
      float val = noise.eval(x, y);
      EXPECT_GE(val, -1.1f);
      EXPECT_LE(val, 1.1f);
    }
  }
}

// --- 3D Simplex Noise Tests ---

TEST(Simplex3DTest, OriginIsZero) {
  lightrand::generator gen(42);
  lightrand::simplex3d<> noise(gen);

  EXPECT_NEAR(noise.eval(0.0f, 0.0f, 0.0f), 0.0f, EPSILON);
}

TEST(Simplex3DTest, Determinism) {
  lightrand::generator gen1(999);
  lightrand::simplex3d<> noise1(gen1);

  lightrand::generator gen2(999);
  lightrand::simplex3d<> noise2(gen2);

  EXPECT_FLOAT_EQ(noise1.eval(1.23f, 4.56f, 7.89f),
                  noise2.eval(1.23f, 4.56f, 7.89f));
  EXPECT_FLOAT_EQ(noise1.eval(-0.12f, 3.45f, -6.78f),
                  noise2.eval(-0.12f, 3.45f, -6.78f));
}

TEST(Simplex3DTest, Bounds) {
  lightrand::generator gen(12345);
  lightrand::simplex3d<> noise(gen);

  for (float x = -5.0f; x < 5.0f; x += 0.5f) {
    for (float y = -5.0f; y < 5.0f; y += 0.5f) {
      for (float z = -5.0f; z < 5.0f; z += 0.5f) {
        float val = noise.eval(x, y, z);
        EXPECT_GE(val, -1.1f);
        EXPECT_LE(val, 1.1f);
      }
    }
  }
}

} // namespace
