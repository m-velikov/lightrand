#include "lightrand/random.h"
#include <benchmark/benchmark.h>
#include <random>

namespace {

std::mt19937_64 g_mt_eng(12345);
lightrand::xoshiro256starstar g_xo_eng(12345);

// --- PRNG Engines ---

void BM_Engine_MT19937_64(benchmark::State &state) {
  g_mt_eng.seed(12345);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(g_mt_eng());
}
BENCHMARK(BM_Engine_MT19937_64);

void BM_Engine_Xoshiro256StarStar(benchmark::State &state) {
  g_xo_eng.seed(12345);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(g_xo_eng());
}
BENCHMARK(BM_Engine_Xoshiro256StarStar);

// --- Uniform Integer Distribution ---

void BM_StdUniformInt(benchmark::State &state) {
  g_xo_eng.seed(12345);
  std::uniform_int_distribution<int> dist(10, 1000);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(dist(g_xo_eng));
}
BENCHMARK(BM_StdUniformInt);

void BM_StdUniformInt_MT(benchmark::State &state) {
  g_mt_eng.seed(12345);
  std::uniform_int_distribution<int> dist(10, 1000);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(dist(g_mt_eng));
}
BENCHMARK(BM_StdUniformInt_MT);

void BM_LightrandUniformInt(benchmark::State &state) {
  g_xo_eng.seed(12345);
  lightrand::generator gen(g_xo_eng);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(gen.uniform<int>(10, 1000));
}
BENCHMARK(BM_LightrandUniformInt);

// --- Uniform Real Distribution ---

void BM_StdUniformReal(benchmark::State &state) {
  g_xo_eng.seed(12345);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(dist(g_xo_eng));
}
BENCHMARK(BM_StdUniformReal);

void BM_StdUniformReal_MT(benchmark::State &state) {
  g_mt_eng.seed(12345);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(dist(g_mt_eng));
}
BENCHMARK(BM_StdUniformReal_MT);

void BM_LightrandUniformReal(benchmark::State &state) {
  g_xo_eng.seed(12345);
  lightrand::generator gen(g_xo_eng);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(gen.uniform<double>());
}
BENCHMARK(BM_LightrandUniformReal);

// --- Normal Distribution ---

void BM_StdNormal(benchmark::State &state) {
  g_xo_eng.seed(12345);
  std::normal_distribution<double> dist(0.0, 1.0);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(dist(g_xo_eng));
}
BENCHMARK(BM_StdNormal);

void BM_StdNormal_MT(benchmark::State &state) {
  g_mt_eng.seed(12345);
  std::normal_distribution<double> dist(0.0, 1.0);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(dist(g_mt_eng));
}
BENCHMARK(BM_StdNormal_MT);

void BM_LightrandNormal(benchmark::State &state) {
  g_xo_eng.seed(12345);
  lightrand::generator gen(g_xo_eng);
  for (auto _ [[maybe_unused]] : state)
    benchmark::DoNotOptimize(gen.normal<double>());
}
BENCHMARK(BM_LightrandNormal);

} // namespace

BENCHMARK_MAIN();
