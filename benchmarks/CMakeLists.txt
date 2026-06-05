add_executable(random-benchmark random-benchmark.cc)
target_link_libraries(random-benchmark PRIVATE benchmark::benchmark_main lightrand)

add_executable(perlin-benchmark perlin-benchmark.cc)
target_link_libraries(perlin-benchmark PRIVATE benchmark::benchmark_main lightrand)

add_executable(simplex-benchmark simplex-benchmark.cc)
target_link_libraries(simplex-benchmark PRIVATE benchmark::benchmark_main lightrand)
