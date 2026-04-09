#include "lightrand/random.h"
#include <argparse/argparse.hpp>
#include <format>
#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("gen-uniform");

  program.add_description(
      "A utility to generate uniformly distributed floating-point numbers.\n"
      "Outputs numbers in the range [0, 1) to standard output.");

  program.add_argument("-s", "--seed")
      .help("Seed for the random number generator.\n"
            "If omitted or set to 0, a seed from std::random_device will be "
            "used.")
      .scan<'u', uint64_t>();

  program.add_argument("-c", "--count")
      .help("Number of floating-point numbers to generate (default: 10,000,000).")
      .scan<'u', uint64_t>();

  program.add_argument("-d", "--decimal")
      .help("Output decimal floating-point numbers instead of hexadecimal.")
      .default_value(false)
      .implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << "\n";
    std::cerr << program;
    return 1;
  }

  uint64_t seed = program.present<uint64_t>("--seed").value_or(0);
  if (seed == 0) {
    std::random_device rd;
    seed = (static_cast<uint64_t>(rd()) << 32) | rd();
  }

  uint64_t n = program.present<uint64_t>("--count").value_or(10'000'000);
  lightrand::generator gen(seed);

  bool decimal = program.get<bool>("--decimal");

  for (uint64_t i = 0; i < n; ++i) {
    double val = gen.uniform<double>();
    if (decimal) {
      std::cout << std::format("{:.17f}\n", val);
    } else {
      std::cout << std::format("{:a}\n", val);
    }
  }

  return 0;
}
