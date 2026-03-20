
// Generate tables for the Ziggurat algorithm with 256 levels.
#include <cmath>
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

int main() {
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  double f = std::exp(-0.5 * R * R);
  x[0] = V / f;
  x[1] = R;
  x[N] = 0.0;

  for (auto i = 2u; i < N; ++i) {
    x[i] = std::sqrt(-2 * std::log(V / x[i - 1] + f));
    f = std::exp(-0.5 * x[i] * x[i]);
  }

  std::cout << "namespace lightrand {\n"
               "namespace ziggurat {\n\n"
               "extern const double x[] = {\n";
  for (auto i = 0u; i <= N; ++i) {
// Manually prepend 0x to workaround the MSVC std::format {:a} bug
#ifdef _MSC_VER
#define OH_EX "0x"
#else
#define OH_EX ""
#endif
    std::cout << std::format("  " OH_EX "{:a}, // {:.17g}\n", x[i], x[i]);
  }
  std::cout << "};\n\n} // namespace ziggurat\n} // namespace lightrand\n";
}
