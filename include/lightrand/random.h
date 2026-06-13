#pragma once

#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <optional>
#include <random>

namespace lightrand {

namespace detail {
/**
 * @brief Resolves an optional seed to a concrete 64-bit value.
 *
 * Returns the provided seed if present; otherwise draws a fresh seed from
 * std::random_device.
 *
 * @param s An optional seed value.
 * @return The resolved 64-bit seed.
 */
[[nodiscard]] inline std::uint64_t
resolve_seed(std::optional<std::uint64_t> s) {
  if (s)
    return *s;
  std::random_device rd;
  return (static_cast<std::uint64_t>(rd()) << 32) | rd();
}
} // namespace detail

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
  [[nodiscard]] static constexpr std::uint64_t min() noexcept { return 0; }
  [[nodiscard]] static constexpr std::uint64_t max() noexcept {
    return std::numeric_limits<std::uint64_t>::max();
  }

  /**
   * @brief Constructs a SplitMix64 generator with an optional seed.
   *
   * @param s An optional seed value. If std::nullopt, the generator will
   *          be initialized using std::random_device.
   */
  explicit splitmix64(std::optional<std::uint64_t> s = std::nullopt) {
    seed(detail::resolve_seed(s));
  }

  /**
   * @brief Seeds the generator with a new seed.
   *
   * @param s The new seed value.
   */
  void seed(std::uint64_t s) noexcept { state_ = s; }

  /**
   * @brief Generates the next random number.
   *
   * @return The next random number in the sequence.
   */
  std::uint64_t operator()() noexcept {
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
  [[nodiscard]] static constexpr std::uint64_t min() noexcept { return 0; }
  [[nodiscard]] static constexpr std::uint64_t max() noexcept {
    return std::numeric_limits<std::uint64_t>::max();
  }

  /**
   * @brief Constructs a xoshiro256** generator with an optional seed.
   *
   * @param s An optional seed value. If std::nullopt, the generator will
   *             be initialized using std::random_device.
   */
  explicit xoshiro256starstar(std::optional<std::uint64_t> s = std::nullopt) {
    seed(detail::resolve_seed(s));
  }

  /**
   * @brief Seeds the generator with a new seed.
   *
   * @param s The new seed value.
   */
  void seed(std::uint64_t s) noexcept {
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
  std::uint64_t operator()() noexcept {
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
 * @brief Thread-local uniform random bit generator.
 *
 * This is a thread-local instance of the xoshiro256** random number generator.
 *
 * @see xoshiro256starstar
 */
extern thread_local xoshiro256starstar thread_urbg;

namespace ziggurat {
constexpr unsigned N = 256;
constexpr double R = 3.6541528853610088;
extern const std::array<double, N + 1> x;
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
   * @param eng The underlying uniform random bit generator (URBG) engine.
   */
  explicit generator(engine &eng = thread_urbg) noexcept : urbg_(eng) {}

  /**
   * @brief Seeds the underlying random number generator.
   *
   * @param s The new seed value.
   */
  void seed(std::uint64_t s) noexcept { urbg_.seed(s); }

  /**
   * @brief Accesses the underlying uniform random bit generator engine.
   *
   * @return A reference to the underlying engine.
   */
  engine &eng() noexcept { return urbg_; }

  /**
   * @brief Generates a raw 64-bit random integer from the engine.
   *
   * @return A uniformly distributed 64-bit unsigned integer.
   */
  [[nodiscard]] std::uint64_t random() noexcept { return urbg_(); }

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
  [[nodiscard]] T uniform(T lo, T hi = std::numeric_limits<T>::max()) noexcept {
    // Calculate the total range (the distance between lo and hi)
    std::uint64_t range =
        static_cast<std::uint64_t>(hi) - static_cast<std::uint64_t>(lo);
    if (range == std::numeric_limits<std::uint64_t>::max())
      return static_cast<T>(random());

    // Lemire's algorithm and the OpenBSD lower bound method both give numbers
    // in the open interval [lo, hi), but we want [lo, hi].
    range += 1;

#ifdef __SIZEOF_INT128__
    // Daniel Lemire's Fast Random Integer algorithm
    std::uint64_t u = random();
    unsigned __int128 m = static_cast<unsigned __int128>(u) *
                          static_cast<unsigned __int128>(range);
    auto lm = static_cast<std::uint64_t>(m);

    if (lm < range) [[unlikely]] {
      // cppcheck-suppress oppositeExpression
      const std::uint64_t threshold = -range % range;
      while (lm < threshold) {
        u = random();
        m = static_cast<unsigned __int128>(u) *
            static_cast<unsigned __int128>(range);
        lm = static_cast<std::uint64_t>(m);
      }
    }
    return static_cast<T>(static_cast<std::uint64_t>(lo) +
                          static_cast<std::uint64_t>(m >> 64));
#else
    // OpenBSD lower bound rejection method.

    // cppcheck-suppress oppositeExpression
    const std::uint64_t threshold = -range % range;
    std::uint64_t u;
    do {
      u = random();
    } while (u < threshold);
    return static_cast<T>(static_cast<std::uint64_t>(lo) + (u % range));
#endif
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
  template <std::floating_point T> [[nodiscard]] T uniform() noexcept {
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
   * @param hi The upper bound of the range (exclusive). Must be strictly
   *           greater than @p lo: a half-open interval [lo, hi) with lo == hi
   *           is empty and impossible to satisfy.
   * @return A uniformly distributed random floating-point number.
   */
  template <std::floating_point T>
  [[nodiscard]] T uniform(T lo, T hi = static_cast<T>(1.0)) noexcept {
    assert(lo < hi && "uniform(lo, hi) requires lo < hi: [lo, hi) is empty");
    T result = std::lerp(lo, hi, uniform<T>());
    return result == hi ? std::nextafter(hi, lo) : result;
  }

  /**
   * @brief Generates a normally distributed random floating-point number with
   * mean 0 and standard deviation 1.
   *
   * This function uses the Ziggurat algorithm for efficient generation.
   * @tparam T The floating-point type of the generated value.
   * @return A normally distributed random floating-point number.
   */
  template <std::floating_point T> [[nodiscard]] T normal() noexcept {
    const auto &zx = ziggurat::x;

    for (;;) {
      // Choose a uniformly distributed 64-bit integer.
      auto r = random();
      // Bits 0-7 select the box number `i`.
      // Ziggurat N is 256 (power of 2), so we can optimize the index
      // generation without bias.
      auto i = static_cast<unsigned>(r & (ziggurat::N - 1));

      // Bit 8 selects the sign of the result.
      bool negative = (r & ziggurat::N) != 0;

      // Bits 63-11 are interpreted as the mantissa of a `double` in the range
      // [0, 1).
      auto u = static_cast<double>(r >> 11) / 0x1.0p53;

      // If `x` is within the extent of the box above, then the sample point is
      // below the curve.
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
        x = ziggurat::R - p;
        return static_cast<T>(negative ? -x : x);
      }

      // `x` is in an area where the box is partially above the curve.
      // Choose a random `y` between y[i] and y[i+1]
      auto f = [](double x) { return std::exp(-0.5 * x * x); };
      auto y = std::lerp(f(zx[i]), f(zx[i + 1]), uniform<double>());

      // Accept if the point (x, y) is below the curve.
      if (y < f(x))
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
  [[nodiscard]] T normal(T mean, T stddev = static_cast<T>(1.0)) noexcept {
    return mean + stddev * normal<T>();
  }
};

} // namespace lightrand
