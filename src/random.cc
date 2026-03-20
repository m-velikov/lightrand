
#include "lightrand/random.h"
// #include "lightrand/perlin.h"

#include <iostream>
#include <random>

namespace lightrand {

std::ostream &operator<<(std::ostream &os, const splitmix64 &gen) {
  os << gen.state_;
  return os;
}

std::istream &operator>>(std::istream &is, splitmix64 &gen) {
  is >> gen.state_;
  return is;
}

std::ostream &operator<<(std::ostream &os, const xoshiro256starstar &gen) {
  os << gen.state_[0] << " " << gen.state_[1] << " " << gen.state_[2] << " "
     << gen.state_[3];
  return os;
}

std::istream &operator>>(std::istream &is, xoshiro256starstar &gen) {
  is >> gen.state_[0] >> gen.state_[1] >> gen.state_[2] >> gen.state_[3];
  return is;
}

} // namespace lightrand

static_assert(std::uniform_random_bit_generator<lightrand::splitmix64>,
              "The splitmix64 does not meet stdlib requirements for uniform "
              "random bit generartor");

static_assert(
    std::uniform_random_bit_generator<lightrand::xoshiro256starstar>,
    "The xoshiro256starstar does not meet stdlib requirements for uniform "
    "random bit generartor");

lightrand::xoshiro256starstar lightrand::global_urbg;

thread_local lightrand::xoshiro256starstar lightrand::thread_urbg;
