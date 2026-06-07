#pragma once

#include "random.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <type_traits>

namespace lightrand::detail {

// Wraps an cell grid coordinate into the bounds of the tables.
inline std::uint32_t wrap_grid_coord(float coord,
                                     std::uint32_t size_mask) noexcept {
  return static_cast<std::uint32_t>(static_cast<std::int32_t>(coord)) &
         size_mask;
}

struct vec2 {
  static float dot(const vec2 &a, const vec2 &b) noexcept {
    return a.x * b.x + a.y * b.y;
  }
  void normalize() noexcept {
    auto d = dot(*this, *this);
    x /= std::sqrt(d);
    y /= std::sqrt(d);
  }

  float x, y;
};

struct vec3 {
  static float dot(const vec3 &a, const vec3 &b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  void normalize() noexcept {
    auto d = dot(*this, *this);
    x /= std::sqrt(d);
    y /= std::sqrt(d);
    z /= std::sqrt(d);
  }

  float x, y, z;
};

template <typename GradientType, std::uint32_t TABLE_SIZE>
class gradient_table {
  static_assert(std::has_single_bit(TABLE_SIZE),
                "TABLE_SIZE must be a power of 2");

private:
  std::array<GradientType, TABLE_SIZE> grad;
  std::array<std::uint32_t, TABLE_SIZE * 2ull> perm;

public:
  explicit gradient_table(generator &gen) noexcept {
    init_gradients(gen);

    std::iota(perm.begin(), perm.begin() + TABLE_SIZE, 0);
    std::shuffle(perm.begin(), perm.begin() + TABLE_SIZE, gen.eng());
    std::copy(perm.begin(), perm.begin() + TABLE_SIZE,
              perm.begin() + TABLE_SIZE);
  }

  const GradientType &operator()(std::uint32_t hx) const noexcept {
    return grad[perm[hx]];
  }

  const GradientType &operator()(std::uint32_t hx,
                                 std::uint32_t hy) const noexcept {
    return grad[perm[perm[hx] + hy]];
  }

  const GradientType &operator()(std::uint32_t hx, std::uint32_t hy,
                                 std::uint32_t hz) const noexcept {
    return grad[perm[perm[perm[hx] + hy] + hz]];
  }

private:
  void init_gradients(generator &gen) noexcept {
    if constexpr (std::is_same_v<GradientType, float>) {
      for (auto &g : grad) {
        do {
          g = gen.normal<float>();
        } while (g == 0.0f);
        g = std::copysign(1.0f, g);
      }
    } else if constexpr (std::is_same_v<GradientType, vec2>) {
      for (auto &g : grad) {
        do {
          g = {.x = gen.normal<float>(), .y = gen.normal<float>()};
        } while (g.x == 0.0f && g.y == 0.0f);
        g.normalize();
      }
    } else if constexpr (std::is_same_v<GradientType, vec3>) {
      for (auto &g : grad) {
        do {
          g = {.x = gen.normal<float>(),
               .y = gen.normal<float>(),
               .z = gen.normal<float>()};
        } while (g.x == 0.0f && g.y == 0.0f && g.z == 0.0f);
        g.normalize();
      }
    }
  }
};

} // namespace lightrand::detail
