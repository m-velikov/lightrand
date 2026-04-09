#include <algorithm>
#include <argparse/argparse.hpp>
#include <iostream>
#include <lightrand/random.h>
#include <random>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char *argv[]) {
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
      .scan<'u', uint64_t>();

  try {
    args.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << "\n";
    std::cerr << args;
    return 1;
  }

#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  uint64_t seed = args.present<uint64_t>("--seed").value_or(0);
  if (seed == 0) {
    std::random_device rd;
    seed = (static_cast<uint64_t>(rd()) << 32) | rd();
  }
  lightrand::xoshiro256starstar rng(seed);

  std::vector<uint64_t> buf;
  uint64_t left = args.present<uint64_t>("--count").value_or(8192);
  bool infinite = (left == 0);
  while (infinite || left > 0) {
    uint64_t n = infinite ? 8192 : std::min(uint64_t(8192), left);
    buf.resize(n);

    std::generate(buf.begin(), buf.end(), [&rng]() { return rng(); });
    std::cout.write(reinterpret_cast<const char *>(buf.data()),
                    n * sizeof(uint64_t));

    if (!infinite)
      left -= n;
  }

  return 0;
}
