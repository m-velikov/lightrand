#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>

namespace lightrand {

/**
 * @brief SplitMix64 random number generator.
 *
 * This class implements the SplitMix64 algorithm, a fast and simple
 * pseudorandom number generator (PRNG). It meets the C++ standard
 * requirements for a uniform random bit generator.
 *
 * SplitMix64 is primarily used for seeding other PRNGs, but it can also be
 * used as a standalone generator in applications where speed and simplicity
 * are more important than statistical quality. It is a 64-bit generator
 * with a period of 2^64.
 *
 * @see https://prng.di.unimi.it/splitmix64.c
 */
class splitmix64 {
private:
  friend std::ostream &operator<<(std::ostream &, const splitmix64 &);
  friend std::istream &operator>>(std::istream &, splitmix64 &);

  std::uint64_t state_;

public:
  using result_type = std::uint64_t;
  static constexpr std::uint64_t min() { return 0; }
  static constexpr std::uint64_t max() {
    return std::numeric_limits<std::uint64_t>::max();
  }

  /**
   * @brief Constructs a SplitMix64 generator with an optional seed.
   *
   * @param s An optional seed value. If not provided, the generator will
   *          be initialized with a default seed.
   */
  explicit splitmix64(std::uint64_t s = 0xcafebabe) { seed(s); }

  /**
   * @brief Seeds the generator with a new seed.
   *
   * @param s The new seed value.
   */
  void seed(std::uint64_t s) { state_ = s; }

  /**
   * @brief Generates the next random number.
   *
   * @return The next random number in the sequence.
   */
  std::uint64_t operator()() {
    std::uint64_t z = (state_ += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
  }
};

/**
 * @brief xoshiro256** random number generator.
 *
 * This class implements the xoshiro256** algorithm, a fast and high-quality
 * pseudorandom number generator (PRNG). It meets the C++ standard
 * requirements for a uniform random bit generator.
 *
 * The xoshiro256** algorithm is a member of the xoshiro family of PRNGs,
 * known for their speed and statistical quality. It is a 256-bit generator
 * with a period of 2^256 - 1.
 *
 * @see https://prng.di.unimi.it/xoshiro256starstar.c
 */
class xoshiro256starstar {
private:
  std::array<std::uint64_t, 4> state_;

  friend std::ostream &operator<<(std::ostream &, const xoshiro256starstar &);
  friend std::istream &operator>>(std::istream &, xoshiro256starstar &);

public:
  using type = std::uint64_t;
  using result_type = std::uint64_t;
  static constexpr std::uint64_t min() { return 0; }
  static constexpr std::uint64_t max() {
    return std::numeric_limits<std::uint64_t>::max();
  }

  /**
   * @brief Constructs a xoshiro256** generator with an optional seed.
   *
   * @param s An optional seed value. If not provided, the generator will
   *             be initialized with a default seed.
   */
  explicit xoshiro256starstar(std::uint64_t s = 0xc0ffeef00d) { seed(s); }

  /**
   * @brief Seeds the generator with a new seed.
   *
   * @param s The new seed value.
   */
  void seed(std::uint64_t s) {
    splitmix64 init(s);
    state_[0] = init();
    state_[1] = init();
    state_[2] = init();
    state_[3] = init();
  }

  /**
   * @brief Generates the next random number.
   *
   * @return The next random number in the sequence.
   */
  std::uint64_t operator()() {
    const std::uint64_t res = std::rotl(state_[1] * 5, 7) * 9;

    const std::uint64_t t = state_[1] << 17;

    state_[2] ^= state_[0];
    state_[3] ^= state_[1];
    state_[1] ^= state_[2];
    state_[0] ^= state_[3];

    state_[2] ^= t;

    state_[3] = std::rotl(state_[3], 45);

    return res;
  }
};

using engine = xoshiro256starstar;

/**
 * @brief Global uniform random bit generator.
 *
 * This is a global instance of the xoshiro256** random number generator,
 * used as the default uniform random bit generator (URBG) throughout the
 * library. It provides a high-quality source of random bits for various
 * random number distributions.
 *
 * The `xoshiro256starstar` algorithm is a fast and statistically robust
 * pseudorandom number generator (PRNG) suitable for general-purpose use.
 *
 * This global instance is intended to be used as a default URBG
 * across different parts of the application.
 *
 * @see xoshiro256starstar
 */
extern xoshiro256starstar global_urbg;

extern thread_local xoshiro256starstar thread_urbg;

namespace ziggurat {
constexpr unsigned N = 256;
constexpr double R = 3.6541528853610088;
extern const double x[];
} // namespace ziggurat

namespace detail {
template <typename Derived> class random_generator {
protected:
  random_generator() = default;

public:
  std::uint64_t random() { return static_cast<Derived *>(this)->random(); }

  template <std::integral T> T uniform(T lo, T hi) {
    if (lo >= hi)
      return lo;

    std::uint64_t range =
        static_cast<std::uint64_t>(hi) - static_cast<std::uint64_t>(lo);
    if (range == std::numeric_limits<std::uint64_t>::max())
      return static_cast<T>(random());

    std::uint64_t limit =
        (std::numeric_limits<std::uint64_t>::max() / (range + 1)) * (range + 1);

    std::uint64_t u;
    do {
      u = random();
    } while (u >= limit);
    return static_cast<T>(static_cast<std::uint64_t>(lo) + (u % (range + 1)));
  }

  template <std::floating_point T> T uniform() {
    constexpr int bits = std::numeric_limits<T>::digits < 64
                             ? std::numeric_limits<T>::digits
                             : 64;
    constexpr int shift = 64 - bits;
    constexpr T divisor = static_cast<T>(1ULL << (bits - 1)) * 2.0;

    return static_cast<T>(random() >> shift) / divisor;
  }

  template <std::floating_point T> T uniform(T lo, T hi) {
    if (lo >= hi)
      return lo;

    return lo + uniform<T>() * (hi - lo);
  }

  /**
   * @brief Generates a normally distributed random floating-point number with
   * mean 0 and standard deviation 1.
   *
   * This function uses the Ziggurat algorithm for efficient generation.
   * @tparam T The floating-point type of the generated value.
   * @return A normally distributed random floating-point number.
   */
  template <std::floating_point T> T normal() {
    const auto &zx = ziggurat::x;
    for (;;) {
      // Choose the box number `i`.
      // Ziggurat N is 256 (power of 2), so we can optimize the index
      // generation.
      auto i = static_cast<unsigned>(random() & (ziggurat::N - 1));

      // Choose an `x` coordinate.
      auto u = 2.0 * uniform<double>() - 1.0;
      auto x = u * zx[i];

      // If `x` is below `zx[i+1]`, it's certainly below the curve.
      if (std::fabs(x) < zx[i + 1])
        return static_cast<T>(x);

      if (i == 0) {
        // Marsaglia's polar method for the tail
        double p, q;
        do {
          // Use 1.0 - uniform() to yield (0, 1] avoiding std::log(0)
          p = std::log(1.0 - uniform<double>()) / ziggurat::R;
          q = std::log(1.0 - uniform<double>());
        } while (-2.0 * q < p * p);
        return static_cast<T>((u < 0) ? p - ziggurat::R : ziggurat::R - p);
      }

      // `x` is in an area where the box is partially above the curve.
      // Choose a random `y` between y[i] and y[i+1]
      u = uniform<double>();
      auto f0 = std::exp(-0.5 * zx[i] * zx[i]);
      auto f1 = std::exp(-0.5 * zx[i + 1] * zx[i + 1]);
      auto y = f0 + u * (f1 - f0);

      // Accept if the point (x, y) is below the curve.
      if (y < std::exp(-0.5 * x * x))
        return static_cast<T>(x);
    }
  }

  /**
   * @brief Generates a normally distributed random floating-point number with
   * the specified mean and standard deviation.
   * @tparam T The floating-point type of the generated value.
   * @param mean The mean of the distribution.
   * @param stddev The standard deviation of the distribution.
   * @return A normally distributed random floating-point number.
   */
  template <std::floating_point T> T normal(T mean, T stddev) {
    return mean + stddev * normal<T>();
  }
};
} // namespace detail

class generator : public detail::random_generator<generator> {
protected:
  engine &urbg_;

public:
  generator(std::uint64_t s = 0xc0ffeef00d, engine &eng = global_urbg)
      : urbg_(eng) {
    seed(s);
  }

  void seed(std::uint64_t s) { urbg_.seed(s); }

  engine &eng() { return urbg_; }

  std::uint64_t random() { return urbg_(); }
};

} // namespace lightrand
