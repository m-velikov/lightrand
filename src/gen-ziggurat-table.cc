// Generate tables for the Ziggurat algorithm with 256 levels.
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <format>
#include <iostream>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h> // NOLINT(misc-include-cleaner)
#endif

constexpr double R = 3.6541528853610088;
constexpr double V = 0.00492867323399;

constexpr unsigned N = 256;
namespace {
std::array<double, N + 1> x;
std::array<double, N + 1> y;
std::array<std::uint64_t, N> k;
std::array<double, N + 1> w;
} // namespace

int main() {
  try {
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY); // NOLINT(misc-include-cleaner)
#endif

    auto f = [](double x) { return std::exp(-0.5 * x * x); };

    x[0] = V / f(R);
    y[0] = f(x[0]);

    x[1] = R;
    y[1] = f(x[1]);

    x[N] = 0.0;
    y[N] = 1.0;

    for (auto i = 2u; i < N; ++i) {
      x[i] = std::sqrt(-2 * std::log(V / x[i - 1] + y[i - 1]));
      y[i] = f(x[i]);
    }

    for (auto i = 0u; i < N; ++i)
      k[i] = static_cast<std::uint64_t>(std::floor(0x1p64 * (x[i + 1] / x[i])));

    for (auto i = 0u; i <= N; ++i)
      w[i] = x[i] / 0x1p64;

    std::cout << "#include <array>\n\n"
                 "namespace lightrand::ziggurat {\n\n";
    std::cout << "// NOLINTBEGIN(modernize-use-std-numbers)\n";
    std::cout << std::format("extern const std::array<double, {}> x = {{\n",
                             N + 1);
    for (auto i = 0u; i <= N; ++i)
      std::cout << std::format("  0x{:a}, // {:.17g}\n", x[i], x[i]);
    std::cout << "};\n"
                 "// NOLINTEND(modernize-use-std-numbers)\n\n";

    std::cout << "} // namespace lightrand::ziggurat\n";
  } catch (const std::exception &err) {
    std::fprintf(stderr, "%s\n", err.what());
    return 1;
  } catch (...) {
    return 1;
  }
}
