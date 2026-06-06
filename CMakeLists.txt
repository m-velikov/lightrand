cmake_minimum_required(VERSION 3.28)

project(lightrand LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()
find_package(benchmark REQUIRED)
find_package(argparse REQUIRED)

add_library(lightrand STATIC src/random.cc src/ziggurat-data.cc)

target_include_directories(lightrand PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)

install(TARGETS lightrand
    EXPORT lightrand-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT lightrand-targets
    FILE lightrand-targets.cmake
    NAMESPACE lightrand::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lightrand
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lightrand-config.cmake"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/lightrand-targets.cmake\")\n"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lightrand-config.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lightrand
)

add_executable(gen-ziggurat-table src/gen-ziggurat-table.cc)

add_executable(random-test src/random-test.cc)
target_link_libraries(random-test PRIVATE GTest::gtest_main lightrand)
gtest_discover_tests(random-test)

add_executable(random-benchmark src/random-benchmark.cc)
target_link_libraries(random-benchmark PRIVATE benchmark::benchmark_main lightrand)

add_executable(perlin-test src/perlin-test.cc)
target_link_libraries(perlin-test PRIVATE GTest::gtest_main lightrand)
gtest_discover_tests(perlin-test)

add_executable(perlin-benchmark src/perlin-benchmark.cc)
target_link_libraries(perlin-benchmark PRIVATE benchmark::benchmark_main lightrand)

add_executable(simplex-test src/simplex-test.cc)
target_link_libraries(simplex-test PRIVATE GTest::gtest_main lightrand)
gtest_discover_tests(simplex-test)

add_executable(simplex-benchmark src/simplex-benchmark.cc)
target_link_libraries(simplex-benchmark PRIVATE benchmark::benchmark_main lightrand)

add_executable(gen-bits src/gen-bits.cc)
target_link_libraries(gen-bits PRIVATE argparse::argparse lightrand)

add_executable(gen-uniform src/gen-uniform.cc)
target_link_libraries(gen-uniform PRIVATE argparse::argparse lightrand)

add_executable(gen-normal src/gen-normal.cc)
target_link_libraries(gen-normal PRIVATE argparse::argparse lightrand)
