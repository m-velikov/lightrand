#include "gradient-table.h"
#include "random.h"

#include <bit>
#include <cmath>
#include <cstdint>

namespace lightrand {

namespace detail {
inline float smootherstep(float u) noexcept {
  return u * u * u * (u * (6.0f * u - 15.0f) + 10.0f);
}

inline float interpolate(float a, float b, float u) noexcept {
  return std::lerp(a, b, smootherstep(u));
}
} // namespace detail

/**
 * @brief 1-dimensional Perlin noise generator.
 *
 * This class generates 1D Perlin noise. It precomputes a table of random
 * gradients and a permutation table for efficient noise evaluation.
 *
 * @tparam TABLE_SIZE The size of the gradient and permutation tables. Must be a
 * power of 2.
 */
template <std::uint32_t TABLE_SIZE = 256> class perlin1d {
  static_assert(std::has_single_bit(TABLE_SIZE),
                "TABLE_SIZE must be a power of 2");

private:
  detail::gradient_table<float, TABLE_SIZE> grad;

public:
  /**
   * @brief Constructs a 1D Perlin noise generator.
   *
   * @param gen The random number generator used to create the gradients and
   *            permutation table.
   */
  explicit perlin1d(generator &gen) noexcept : grad(gen) {}

  /**
   * @brief Evaluates the 1D Perlin noise at a given coordinate.
   *
   * @param x The x-coordinate to evaluate.
   * @return The noise value at the specified coordinate.
   */
  [[nodiscard]] float eval(float x) const noexcept {
    // Determine grid cell coordinates
    float xi = std::floor(x);
    // Determine relative x position within the cell
    float tx = x - xi;

    const std::uint32_t size_mask = TABLE_SIZE - 1;

    // Apply bitmask to wrap grid coordinates within table bounds
    std::uint32_t rx0 = detail::wrap_grid_coord(xi, size_mask);
    std::uint32_t rx1 = (rx0 + 1) & size_mask;

    // Fetch gradients and compute the dot product between the gradient and
    // distance vector
    float nx0 = tx * grad(rx0);
    float nx1 = (tx - 1.0f) * grad(rx1);

    // Interpolate between the two grid points
    return detail::interpolate(nx0, nx1, tx);
  }
};

/**
 * @brief 2-dimensional Perlin noise generator.
 *
 * This class generates 2D Perlin noise. It precomputes a table of random
 * unit gradients and a permutation table for efficient noise evaluation.
 *
 * @tparam TABLE_SIZE The size of the gradient and permutation tables. Must be a
 * power of 2.
 */
template <std::uint32_t TABLE_SIZE = 256> class perlin2d {
  static_assert(std::has_single_bit(TABLE_SIZE),
                "TABLE_SIZE must be a power of 2");

private:
  detail::gradient_table<detail::vec2, TABLE_SIZE> ctx;

public:
  /**
   * @brief Constructs a 2D Perlin noise generator.
   *
   * @param gen The random number generator used to create the gradients and
   *            permutation table.
   */
  explicit perlin2d(generator &gen) noexcept : ctx(gen) {}

  /**
   * @brief Evaluates the 2D Perlin noise at given coordinates.
   *
   * @param x The x-coordinate to evaluate.
   * @param y The y-coordinate to evaluate.
   * @return The noise value at the specified coordinates.
   */
  [[nodiscard]] float eval(float x, float y) const noexcept {
    // Determine grid cell coordinates
    float xi = std::floor(x);
    float yi = std::floor(y);

    // Determine relative x, y position within the cell
    float tx = x - xi;
    float ty = y - yi;

    const std::uint32_t size_mask = TABLE_SIZE - 1;

    // Apply bitmask to wrap grid coordinates within table bounds
    std::uint32_t rx0 = detail::wrap_grid_coord(xi, size_mask);
    std::uint32_t rx1 = (rx0 + 1) & size_mask;
    std::uint32_t ry0 = detail::wrap_grid_coord(yi, size_mask);
    std::uint32_t ry1 = (ry0 + 1) & size_mask;

    // Fetch gradients at the 4 corners of the grid cell
    const detail::vec2 &c00 = ctx(rx0, ry0);
    const detail::vec2 &c10 = ctx(rx1, ry0);
    const detail::vec2 &c01 = ctx(rx0, ry1);
    const detail::vec2 &c11 = ctx(rx1, ry1);

    // Compute distance vectors from the point to the 4 corners
    float dx0 = tx;
    float dx1 = tx - 1.0f;
    float dy0 = ty;
    float dy1 = ty - 1.0f;

    detail::vec2 p00 = {.x = dx0, .y = dy0};
    detail::vec2 p10 = {.x = dx1, .y = dy0};
    detail::vec2 p01 = {.x = dx0, .y = dy1};
    detail::vec2 p11 = {.x = dx1, .y = dy1};

    // Compute the dot product between the gradient and distance vector, then
    // interpolate along x
    float nx0 = detail::interpolate(detail::vec2::dot(p00, c00),
                                    detail::vec2::dot(p10, c10), tx);
    float nx1 = detail::interpolate(detail::vec2::dot(p01, c01),
                                    detail::vec2::dot(p11, c11), tx);

    // Interpolate along y
    return detail::interpolate(nx0, nx1, ty);
  }
};

/**
 * @brief 3-dimensional Perlin noise generator.
 *
 * This class generates 3D Perlin noise. It precomputes a table of random
 * unit gradients and a permutation table for efficient noise evaluation.
 *
 * @tparam TABLE_SIZE The size of the gradient and permutation tables. Must be a
 * power of 2.
 */
template <std::uint32_t TABLE_SIZE = 256> class perlin3d {
  static_assert(std::has_single_bit(TABLE_SIZE),
                "TABLE_SIZE must be a power of 2");

private:
  detail::gradient_table<detail::vec3, TABLE_SIZE> ctx;

public:
  /**
   * @brief Constructs a 3D Perlin noise generator.
   *
   * @param gen The random number generator used to create the gradients and
   *            permutation table.
   */
  explicit perlin3d(generator &gen) noexcept : ctx(gen) {}

  /**
   * @brief Evaluates the 3D Perlin noise at given coordinates.
   *
   * @param x The x-coordinate to evaluate.
   * @param y The y-coordinate to evaluate.
   * @param z The z-coordinate to evaluate.
   * @return The noise value at the specified coordinates.
   */
  [[nodiscard]] float eval(float x, float y, float z) const noexcept {
    // Determine grid cell coordinates
    float xi = std::floor(x);
    float yi = std::floor(y);
    float zi = std::floor(z);

    // Determine relative x, y, z position within the cell
    float tx = x - xi;
    float ty = y - yi;
    float tz = z - zi;

    const std::uint32_t size_mask = TABLE_SIZE - 1;

    // Apply bitmask to wrap grid coordinates within table bounds
    std::uint32_t rx0 = detail::wrap_grid_coord(xi, size_mask);
    std::uint32_t rx1 = (rx0 + 1) & size_mask;
    std::uint32_t ry0 = detail::wrap_grid_coord(yi, size_mask);
    std::uint32_t ry1 = (ry0 + 1) & size_mask;
    std::uint32_t rz0 = detail::wrap_grid_coord(zi, size_mask);
    std::uint32_t rz1 = (rz0 + 1) & size_mask;

    // Fetch gradients at the 8 corners of the grid cube
    const detail::vec3 &c000 = ctx(rx0, ry0, rz0);
    const detail::vec3 &c100 = ctx(rx1, ry0, rz0);
    const detail::vec3 &c010 = ctx(rx0, ry1, rz0);
    const detail::vec3 &c110 = ctx(rx1, ry1, rz0);

    const detail::vec3 &c001 = ctx(rx0, ry0, rz1);
    const detail::vec3 &c101 = ctx(rx1, ry0, rz1);
    const detail::vec3 &c011 = ctx(rx0, ry1, rz1);
    const detail::vec3 &c111 = ctx(rx1, ry1, rz1);

    // Compute distance vectors from the point to the 8 corners
    float dx0 = tx;
    float dx1 = tx - 1.0f;
    float dy0 = ty;
    float dy1 = ty - 1.0f;
    float dz0 = tz;
    float dz1 = tz - 1.0f;

    detail::vec3 p000 = {.x = dx0, .y = dy0, .z = dz0};
    detail::vec3 p100 = {.x = dx1, .y = dy0, .z = dz0};
    detail::vec3 p010 = {.x = dx0, .y = dy1, .z = dz0};
    detail::vec3 p110 = {.x = dx1, .y = dy1, .z = dz0};

    detail::vec3 p001 = {.x = dx0, .y = dy0, .z = dz1};
    detail::vec3 p101 = {.x = dx1, .y = dy0, .z = dz1};
    detail::vec3 p011 = {.x = dx0, .y = dy1, .z = dz1};
    detail::vec3 p111 = {.x = dx1, .y = dy1, .z = dz1};

    // Compute the dot product between the gradient and distance vector, then
    // interpolate along x
    float nx0 = detail::interpolate(detail::vec3::dot(p000, c000),
                                    detail::vec3::dot(p100, c100), tx);
    float nx1 = detail::interpolate(detail::vec3::dot(p010, c010),
                                    detail::vec3::dot(p110, c110), tx);
    float nx2 = detail::interpolate(detail::vec3::dot(p001, c001),
                                    detail::vec3::dot(p101, c101), tx);
    float nx3 = detail::interpolate(detail::vec3::dot(p011, c011),
                                    detail::vec3::dot(p111, c111), tx);

    // Interpolate along y
    float ny0 = detail::interpolate(nx0, nx1, ty);
    float ny1 = detail::interpolate(nx2, nx3, ty);

    // Interpolate along z
    return detail::interpolate(ny0, ny1, tz);
  }
};

} // namespace lightrand
