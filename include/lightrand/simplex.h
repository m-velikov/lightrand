#pragma once

#include "gradient-table.h"
#include "random.h"

#include <bit>
#include <cmath>
#include <cstdint>

namespace lightrand {

/**
 * @brief 2-dimensional Simplex noise generator.
 *
 * This class generates 2D Simplex noise. It precomputes a table of random
 * unit gradients and a permutation table for efficient noise evaluation.
 * Simplex noise generally has fewer directional artifacts and scales better
 * to higher dimensions than standard Perlin noise.
 *
 * @tparam TABLE_SIZE The size of the gradient and permutation tables. Must be a
 * power of 2.
 */
template <std::uint32_t TABLE_SIZE = 256> class simplex2d {
  static_assert(std::has_single_bit(TABLE_SIZE),
                "TABLE_SIZE must be a power of 2");

private:
  detail::gradient_table<detail::vec2, TABLE_SIZE> grad;

public:
  /**
   * @brief Constructs a 2D Simplex noise generator.
   *
   * @param gen The random number generator used to create the gradients and
   *            permutation table.
   */
  simplex2d(generator &gen) : grad(gen) {}

  /**
   * @brief Evaluates the 2D Simplex noise at given coordinates.
   *
   * @param x The x-coordinate to evaluate.
   * @param y The y-coordinate to evaluate.
   * @return The noise value at the specified coordinates, roughly in the range
   * [-1.0, 1.0].
   */
  float eval(float x, float y) const {
    // Skewing and unskewing factors for 2D
    const float F2 = 0.366025403784439f; // (sqrt(3.0) - 1.0) / 2.0
    const float G2 = 0.211324865405187f; // (3.0 - sqrt(3.0)) / 6.0

    // Skew the input space to determine which simplex cell we're in
    float s = (x + y) * F2;
    float i = std::floor(x + s);
    float j = std::floor(y + s);

    // Unskew the cell origin back to (x,y) space
    float t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;

    // The x,y distances from the cell origin
    float dx0 = x - X0;
    float dy0 = y - Y0;

    // Determine which simplex we are in to find the second corner
    float i1, j1;
    if (dx0 > dy0) {
      i1 = 1.0f;
      j1 = 0.0f; // Lower triangle, XY order
    } else {
      i1 = 0.0f;
      j1 = 1.0f; // Upper triangle, YX order
    }

    // Distances from the second and third corners
    float dx1 = dx0 - i1 + G2;
    float dy1 = dy0 - j1 + G2;
    float dx2 = dx0 - 1.0f + 2.0f * G2;
    float dy2 = dy0 - 1.0f + 2.0f * G2;

    const std::uint32_t size_mask = TABLE_SIZE - 1;

    // Wrap grid coordinates within table bounds
    std::uint32_t hi0 = detail::wrap_grid_coord(i, size_mask);
    std::uint32_t hj0 = detail::wrap_grid_coord(j, size_mask);
    std::uint32_t hi1 = detail::wrap_grid_coord(i + i1, size_mask);
    std::uint32_t hj1 = detail::wrap_grid_coord(j + j1, size_mask);
    std::uint32_t hi2 = detail::wrap_grid_coord(i + 1.0f, size_mask);
    std::uint32_t hj2 = detail::wrap_grid_coord(j + 1.0f, size_mask);

    // Calculate the radial falloff contributions from the three corners
    float n0, n1, n2;

    float t0 = 0.5f - dx0 * dx0 - dy0 * dy0;
    if (t0 < 0.0f) {
      n0 = 0.0f;
    } else {
      t0 *= t0;
      detail::vec2 p0 = {dx0, dy0};
      n0 = t0 * t0 * detail::vec2::dot(grad(hi0, hj0), p0);
    }

    float t1 = 0.5f - dx1 * dx1 - dy1 * dy1;
    if (t1 < 0.0f) {
      n1 = 0.0f;
    } else {
      t1 *= t1;
      detail::vec2 p1 = {dx1, dy1};
      n1 = t1 * t1 * detail::vec2::dot(grad(hi1, hj1), p1);
    }

    float t2 = 0.5f - dx2 * dx2 - dy2 * dy2;
    if (t2 < 0.0f) {
      n2 = 0.0f;
    } else {
      t2 *= t2;
      detail::vec2 p2 = {dx2, dy2};
      n2 = t2 * t2 * detail::vec2::dot(grad(hi2, hj2), p2);
    }

    // Scale the result to [-1, 1]
    return 70.0f * (n0 + n1 + n2);
  }
};

/**
 * @brief 3-dimensional Simplex noise generator.
 *
 * This class generates 3D Simplex noise. It precomputes a table of random
 * unit gradients and a permutation table for efficient noise evaluation.
 *
 * @tparam TABLE_SIZE The size of the gradient and permutation tables. Must be a
 * power of 2.
 */
template <std::uint32_t TABLE_SIZE = 256> class simplex3d {
  static_assert(std::has_single_bit(TABLE_SIZE),
                "TABLE_SIZE must be a power of 2");

private:
  detail::gradient_table<detail::vec3, TABLE_SIZE> ctx;

public:
  /**
   * @brief Constructs a 3D Simplex noise generator.
   *
   * @param gen The random number generator used to create the gradients and
   *            permutation table.
   */
  simplex3d(generator &gen) : ctx(gen) {}

  /**
   * @brief Evaluates the 3D Simplex noise at given coordinates.
   *
   * @param x The x-coordinate to evaluate.
   * @param y The y-coordinate to evaluate.
   * @param z The z-coordinate to evaluate.
   * @return The noise value at the specified coordinates, roughly in the range
   * [-1.0, 1.0].
   */
  float eval(float x, float y, float z) const {
    // Skewing and unskewing factors for 3D
    const float F3 = 1.0f / 3.0f;
    const float G3 = 1.0f / 6.0f;

    // Skew the input space to determine which simplex cell we're in
    float s = (x + y + z) * F3;
    float i = std::floor(x + s);
    float j = std::floor(y + s);
    float k = std::floor(z + s);

    // Unskew the cell origin back to (x,y,z) space
    float t = (i + j + k) * G3;
    float X0 = i - t;
    float Y0 = j - t;
    float Z0 = k - t;

    // The x,y,z distances from the cell origin
    float dx0 = x - X0;
    float dy0 = y - Y0;
    float dz0 = z - Z0;

    // Determine which simplex we are in
    float i1, j1, k1; // Offsets for second corner
    float i2, j2, k2; // Offsets for third corner

    if (dx0 >= dy0) {
      if (dy0 >= dz0) {
        i1 = 1.0f;
        j1 = 0.0f;
        k1 = 0.0f;
        i2 = 1.0f;
        j2 = 1.0f;
        k2 = 0.0f; // X Y Z order
      } else if (dx0 >= dz0) {
        i1 = 1.0f;
        j1 = 0.0f;
        k1 = 0.0f;
        i2 = 1.0f;
        j2 = 0.0f;
        k2 = 1.0f; // X Z Y order
      } else {
        i1 = 0.0f;
        j1 = 0.0f;
        k1 = 1.0f;
        i2 = 1.0f;
        j2 = 0.0f;
        k2 = 1.0f; // Z X Y order
      }
    } else { // dx0 < dy0
      if (dy0 < dz0) {
        i1 = 0.0f;
        j1 = 0.0f;
        k1 = 1.0f;
        i2 = 0.0f;
        j2 = 1.0f;
        k2 = 1.0f; // Z Y X order
      } else if (dx0 < dz0) {
        i1 = 0.0f;
        j1 = 1.0f;
        k1 = 0.0f;
        i2 = 0.0f;
        j2 = 1.0f;
        k2 = 1.0f; // Y Z X order
      } else {
        i1 = 0.0f;
        j1 = 1.0f;
        k1 = 0.0f;
        i2 = 1.0f;
        j2 = 1.0f;
        k2 = 0.0f; // Y X Z order
      }
    }

    // Distances from the second, third, and fourth corners
    float dx1 = dx0 - i1 + G3;
    float dy1 = dy0 - j1 + G3;
    float dz1 = dz0 - k1 + G3;

    float dx2 = dx0 - i2 + 2.0f * G3;
    float dy2 = dy0 - j2 + 2.0f * G3;
    float dz2 = dz0 - k2 + 2.0f * G3;

    float dx3 = dx0 - 1.0f + 3.0f * G3;
    float dy3 = dy0 - 1.0f + 3.0f * G3;
    float dz3 = dz0 - 1.0f + 3.0f * G3;

    const std::uint32_t size_mask = TABLE_SIZE - 1;

    // Wrap grid coordinates within table bounds
    std::uint32_t hi = detail::wrap_grid_coord(i, size_mask);
    std::uint32_t hj = detail::wrap_grid_coord(j, size_mask);
    std::uint32_t hk = detail::wrap_grid_coord(k, size_mask);

    // Calculate the radial falloff contributions from the four corners
    float n0, n1, n2, n3;

    float t0 = 0.6f - dx0 * dx0 - dy0 * dy0 - dz0 * dz0;
    if (t0 < 0.0f) {
      n0 = 0.0f;
    } else {
      t0 *= t0;
      detail::vec3 p0 = {dx0, dy0, dz0};
      n0 = t0 * t0 * detail::vec3::dot(ctx(hi, hj, hk), p0);
    }

    float t1 = 0.6f - dx1 * dx1 - dy1 * dy1 - dz1 * dz1;
    if (t1 < 0.0f) {
      n1 = 0.0f;
    } else {
      t1 *= t1;
      detail::vec3 p1 = {dx1, dy1, dz1};
      n1 = t1 * t1 *
           detail::vec3::dot(ctx(detail::wrap_grid_coord(i + i1, size_mask),
                                 detail::wrap_grid_coord(j + j1, size_mask),
                                 detail::wrap_grid_coord(k + k1, size_mask)),
                             p1);
    }

    float t2 = 0.6f - dx2 * dx2 - dy2 * dy2 - dz2 * dz2;
    if (t2 < 0.0f) {
      n2 = 0.0f;
    } else {
      t2 *= t2;
      detail::vec3 p2 = {dx2, dy2, dz2};
      n2 = t2 * t2 *
           detail::vec3::dot(ctx(detail::wrap_grid_coord(i + i2, size_mask),
                                 detail::wrap_grid_coord(j + j2, size_mask),
                                 detail::wrap_grid_coord(k + k2, size_mask)),
                             p2);
    }

    float t3 = 0.6f - dx3 * dx3 - dy3 * dy3 - dz3 * dz3;
    if (t3 < 0.0f) {
      n3 = 0.0f;
    } else {
      t3 *= t3;
      detail::vec3 p3 = {dx3, dy3, dz3};
      n3 = t3 * t3 *
           detail::vec3::dot(ctx(detail::wrap_grid_coord(i + 1.0f, size_mask),
                                 detail::wrap_grid_coord(j + 1.0f, size_mask),
                                 detail::wrap_grid_coord(k + 1.0f, size_mask)),
                             p3);
    }

    // Scale the result to [-1, 1]
    return 32.0f * (n0 + n1 + n2 + n3);
  }
};

} // namespace lightrand
