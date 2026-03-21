
// Generate tables for the Ziggurat algorithm with 256 levels.
#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

constexpr double R = 3.6541528853610088;
constexpr double V = 0.00492867323399;

constexpr unsigned N = 256;
double x[N + 1];
double y[N + 1];
std::uint64_t k[N];
double w[N + 1];

int main() {
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
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

  std::cout << "#include <cstdint>\n\n"
               "namespace lightrand {\n"
               "namespace ziggurat {\n\n";

  std::cout << "extern const double x[] = {\n";
  for (auto i = 0u; i <= N; ++i)
    std::cout << std::format("  0x{:a}, // {:.17g}\n", x[i], x[i]);
  std::cout << "};\n\n";

  std::cout << "extern const double y[] = {\n";
  for (auto i = 0u; i <= N; ++i)
    std::cout << std::format("  0x{:a}, // {:.17g}\n", y[i], y[i]);
  std::cout << "};\n\n";
#if 0
// The `k`  and `w` tables are not used in the current implementation
  std::cout << "extern const std::uint64_t k[] = {\n";
  for (auto i = 0u; i < N; ++i)
    std::cout << std::format("  0x{:x},\n", k[i]);
  std::cout << "};\n\n";

  std::cout << "extern const double w[] = {\n";
  for (auto i = 0u; i <= N; ++i)
    std::cout << std::format("  0x{:a}, // {:.17g}\n", w[i], w[i]);
  std::cout << "};\n\n";
#endif
  std::cout << "} // namespace ziggurat\n} // namespace lightrand\n";
}
