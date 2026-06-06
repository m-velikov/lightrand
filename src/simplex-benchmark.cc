#include "lightrand/random.h"
#include "lightrand/simplex.h"
#include <benchmark/benchmark.h>

namespace {

// --- 2D Simplex Benchmarks ---

void BM_Simplex2D_Construct(benchmark::State &state) {
  lightrand::generator gen;
  for (auto _ : state) {
    lightrand::simplex2d<> noise(gen);
    benchmark::DoNotOptimize(noise);
  }
}
BENCHMARK(BM_Simplex2D_Construct);

void BM_Simplex2D_Eval(benchmark::State &state) {
  lightrand::generator gen;
  lightrand::simplex2d<> noise(gen);
  float x = 0.1f, y = 0.2f;
  for (auto _ : state) {
    benchmark::DoNotOptimize(noise.eval(x, y));
    x += 0.01f;
    y += 0.01f;
  }
}
BENCHMARK(BM_Simplex2D_Eval);

// --- 3D Simplex Benchmarks ---

void BM_Simplex3D_Construct(benchmark::State &state) {
  lightrand::generator gen;
  for (auto _ : state) {
    lightrand::simplex3d<> noise(gen);
    benchmark::DoNotOptimize(noise);
  }
}
BENCHMARK(BM_Simplex3D_Construct);

void BM_Simplex3D_Eval(benchmark::State &state) {
  lightrand::generator gen;
  lightrand::simplex3d<> noise(gen);
  float x = 0.1f, y = 0.2f, z = 0.3f;
  for (auto _ : state) {
    benchmark::DoNotOptimize(noise.eval(x, y, z));
    x += 0.01f;
    y += 0.01f;
    z += 0.01f;
  }
}
BENCHMARK(BM_Simplex3D_Eval);

} // namespace
