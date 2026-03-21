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
extern const double y[];
} // namespace ziggurat

/**
 * @brief A high-level random number generator interface.
 *
 * This class provides convenient methods to generate uniformly and
 * normally distributed random numbers (both integers and floating-point).
 * It wraps around an underlying uniform random bit generator (URBG) engine.
 */
class generator {
protected:
  engine &urbg_;

public:
  /**
   * @brief Constructs a random number generator.
   *
   * @param s An optional seed value used to initialize the generator.
   * @param eng The underlying uniform random bit generator (URBG) engine.
   */
  generator(std::uint64_t s = 0xc0ffeef00d, engine &eng = global_urbg)
      : urbg_(eng) {
    seed(s);
  }

  /**
   * @brief Seeds the underlying random number generator.
   *
   * @param s The new seed value.
   */
  void seed(std::uint64_t s) { urbg_.seed(s); }

  /**
   * @brief Accesses the underlying uniform random bit generator engine.
   *
   * @return A reference to the underlying engine.
   */
  engine &eng() { return urbg_; }

  /**
   * @brief Generates a raw 64-bit random integer from the engine.
   *
   * @return A uniformly distributed 64-bit unsigned integer.
   */
  std::uint64_t random() { return urbg_(); }

  /**
   * @brief Generates a uniformly distributed random integer in the closed
   * interval [lo, hi].
   *
   * This function uses rejection sampling to guarantee a perfect uniform
   * distribution and completely eliminate modulo bias.
   *
   * @tparam T The integral type of the generated value.
   * @param lo The lower bound of the range (inclusive).
   * @param hi The upper bound of the range (inclusive).
   * @return A uniformly distributed random integer.
   */
  template <std::integral T>
  T uniform(T lo, T hi = std::numeric_limits<T>::max()) {
    // Calculate the total number of bounds (the distance between lo and hi)
    std::uint64_t range =
        static_cast<std::uint64_t>(hi) - static_cast<std::uint64_t>(lo);
    if (range == std::numeric_limits<std::uint64_t>::max())
      return static_cast<T>(random());

    // Calculate the highest multiple of (range + 1) that fits into a uint64_t.
    // Numbers above or equal to this limit are rejected to avoid modulo bias.
    std::uint64_t limit =
        (std::numeric_limits<std::uint64_t>::max() / (range + 1)) * (range + 1);

    std::uint64_t u;
    do {
      u = random();
    } while (u >= limit);
    return static_cast<T>(static_cast<std::uint64_t>(lo) + (u % (range + 1)));
  }

  /**
   * @brief Generates a uniformly distributed random floating-point number in
   * [0.0, 1.0).
   *
   * Carefully extracts the correct number of random bits based on the requested
   * floating-point type's mantissa to safely avoid precision loss or rounding
   * bugs.
   *
   * @tparam T The floating-point type of the generated value.
   * @return A uniformly distributed random floating-point number.
   */
  template <std::floating_point T> T uniform() {
    // Use exactly the number of bits that the floating point type's mantissa
    // can represent
    constexpr int bits = std::numeric_limits<T>::digits < 64
                             ? std::numeric_limits<T>::digits
                             : 64;
    // Number of bits to discard from the 64-bit random integer
    constexpr int shift = 64 - bits;
    // The divisor needed to normalize the integer into the [0, 1) range
    constexpr T divisor = static_cast<T>(1ULL << (bits - 1)) * 2.0;

    return static_cast<T>(random() >> shift) / divisor;
  }

  /**
   * @brief Generates a uniformly distributed random floating-point number in
   * the interval [lo, hi).
   *
   * @tparam T The floating-point type of the generated value.
   * @param lo The lower bound of the range (inclusive).
   * @param hi The upper bound of the range (exclusive).
   * @return A uniformly distributed random floating-point number.
   */
  template <std::floating_point T> T uniform(T lo, T hi = static_cast<T>(1.0)) {
    return std::lerp(lo, hi, uniform<T>());
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
    const auto &zy = ziggurat::y;

    for (;;) {
      // Choose a uniformly distributed 64-bit integer.
      auto r = random();
      // Bits 0-7 select the box number `i`.
      // Ziggurat N is 256 (power of 2), so we can optimize the index
      // generation without bias.
      auto i = static_cast<unsigned>(r & (ziggurat::N - 1));

      // Bit 8 selects the sign of the result.
      // auto sign = (r & ziggurat::N) ? T{-1.0} : T{1.0};
      bool negative = (r & ziggurat::N) != 0;

      // Bits 63-11 are interpreted as the mantissa of a `double` in the range
      // [0, 1).
      auto u = static_cast<double>(r >> 11) / 0x1.0p53;

      auto x = u * zx[i];
      if (x < zx[i + 1]) [[likely]]
        return static_cast<T>(negative ? -x : x);

      if (i == 0) [[unlikely]] {
        // Marsaglia's polar method for the tail
        double p, q;
        do {
          // Use 1.0 - uniform() to yield (0, 1] avoiding std::log(0)
          p = std::log(1.0 - uniform<double>()) / ziggurat::R;
          q = std::log(1.0 - uniform<double>());
        } while (-2.0 * q < p * p);
        auto x = ziggurat::R - p;
        return static_cast<T>(negative ? -x : x);
      }

      // `x` is in an area where the box is partially above the curve.
      // Choose a random `y` between y[i] and y[i+1]
      auto y = std::lerp(zy[i], zy[i + 1], uniform<double>());

      // Accept if the point (x, y) is below the curve.
      if (y < std::exp(-0.5 * x * x))
        return static_cast<T>(negative ? -x : x);
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
  template <std::floating_point T>
  T normal(T mean, T stddev = static_cast<T>(1.0)) {
    return mean + stddev * normal<T>();
  }
};

} // namespace lightrand
