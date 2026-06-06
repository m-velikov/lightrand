#include "lightrand/random.h"

#include <argparse/argparse.hpp>

#include <cstdint>
#include <cstdio>
#include <exception>
#include <format>
#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
  try {
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
        .help("Number of floating-point numbers to generate (default: "
              "10,000,000).")
        .scan<'u', uint64_t>();

    program.add_argument("-d", "--decimal")
        .help("Output decimal floating-point numbers instead of hexadecimal.")
        .default_value(false)
        .implicit_value(true);

    program.parse_args(argc, argv);

    uint64_t seed = program.present<uint64_t>("--seed").value_or(0);
    if (seed == 0) {
      std::random_device rd;
      seed = (static_cast<uint64_t>(rd()) << 32) | rd();
    }
    uint64_t n = program.present<uint64_t>("--count").value_or(10'000'000);
    bool decimal = program.get<bool>("--decimal");

    lightrand::generator gen;
    gen.seed(seed);
    for (uint64_t i = 0; i < n; ++i) {
      auto val = gen.uniform<double>();
      if (decimal)
        std::cout << std::format("{:.17g}\n", val);
      else
        std::cout << std::format("{:a}\n", val);
    }

    return 0;
  } catch (const std::exception &err) {
    std::fprintf(stderr, "%s\n", err.what());
    return 1;
  } catch (...) {
    return 1;
  }
}
