#include <lightrand/random.h>

#include <argparse/argparse.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <random>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h> // NOLINT(misc-include-cleaner)
#endif

int main(int argc, char *argv[]) {
  try {
    argparse::ArgumentParser args("gen-bits");

    args.add_description(
        "A utility to generate pseudo-random bits using xoshiro256**.\n"
        "Outputs raw binary data to standard output.");

    args.add_argument("-s", "--seed")
        .help("Seed for the random number generator.\n"
              "If omitted or set to 0, a seed from std::random_device will be "
              "used.")
        .scan<'u', uint64_t>();

    args.add_argument("-c", "--count")
        .help("Number of elements to generate.\n"
              "Each element is a 64-bit unsigned integer.\n"
              "If set to 0, generates indefinitely.")
        .scan<'i', int64_t>();

    args.parse_args(argc, argv);

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY); // NOLINT(misc-include-cleaner)
#endif

    uint64_t seed = args.present<uint64_t>("--seed").value_or(0);
    if (seed == 0) {
      std::random_device rd;
      seed = (static_cast<uint64_t>(rd()) << 32) | rd();
    }
    lightrand::xoshiro256starstar rng(seed);

    int64_t left = args.present<int64_t>("--count").value_or(8192);
    if (left < 0) {
      std::cerr << "Count must be non-negative.\n";
      return 1;
    }

    std::vector<uint64_t> buf;
    const bool infinite = (left == 0);
    while (infinite || left > 0) {
      int64_t n = infinite ? 8192 : std::min<int64_t>(8192, left);
      buf.resize(n);

      std::ranges::generate(buf, [&rng]() { return rng(); });
      std::cout.write(reinterpret_cast<const char *>(buf.data()), n * 8);

      if (!infinite)
        left -= n;
    }

    return 0;
  } catch (const std::exception &err) {
    std::fprintf(stderr, "%s\n", err.what());
    return 1;
  } catch (...) {
    return 1;
  }
}
