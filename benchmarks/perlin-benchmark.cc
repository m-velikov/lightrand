#include "lightrand/perlin.h"
#include "lightrand/random.h"
#include <benchmark/benchmark.h>

namespace {

// --- 1D Perlin Benchmarks ---

void BM_Perlin1D_Construct(benchmark::State &state) {
  lightrand::generator gen;
  for (auto _ [[maybe_unused]] : state) {
    lightrand::perlin1d<> noise(gen);
    benchmark::DoNotOptimize(noise);
  }
}
BENCHMARK(BM_Perlin1D_Construct);

void BM_Perlin1D_Eval(benchmark::State &state) {
  lightrand::generator gen;
  lightrand::perlin1d<> noise(gen);
  float x = 0.1f;
  for (auto _ [[maybe_unused]] : state) {
    benchmark::DoNotOptimize(noise.eval(x));
    x += 0.01f;
  }
}
BENCHMARK(BM_Perlin1D_Eval);

// --- 2D Perlin Benchmarks ---

void BM_Perlin2D_Construct(benchmark::State &state) {
  lightrand::generator gen;
  for (auto _ [[maybe_unused]] : state) {
    lightrand::perlin2d<> noise(gen);
    benchmark::DoNotOptimize(noise);
  }
}
BENCHMARK(BM_Perlin2D_Construct);

void BM_Perlin2D_Eval(benchmark::State &state) {
  lightrand::generator gen;
  lightrand::perlin2d<> noise(gen);
  float x = 0.1f, y = 0.2f;
  for (auto _ [[maybe_unused]] : state) {
    benchmark::DoNotOptimize(noise.eval(x, y));
    x += 0.01f;
    y += 0.01f;
  }
}
BENCHMARK(BM_Perlin2D_Eval);

// --- 3D Perlin Benchmarks ---

void BM_Perlin3D_Construct(benchmark::State &state) {
  lightrand::generator gen;
  for (auto _ [[maybe_unused]] : state) {
    lightrand::perlin3d<> noise(gen);
    benchmark::DoNotOptimize(noise);
  }
}
BENCHMARK(BM_Perlin3D_Construct);

void BM_Perlin3D_Eval(benchmark::State &state) {
  lightrand::generator gen;
  lightrand::perlin3d<> noise(gen);
  float x = 0.1f, y = 0.2f, z = 0.3f;
  for (auto _ [[maybe_unused]] : state) {
    benchmark::DoNotOptimize(noise.eval(x, y, z));
    x += 0.01f;
    y += 0.01f;
    z += 0.01f;
  }
}
BENCHMARK(BM_Perlin3D_Eval);

} // namespace
