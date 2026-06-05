include(GoogleTest)

add_executable(random-test random-test.cc)
target_link_libraries(random-test PRIVATE GTest::gtest_main lightrand)
gtest_discover_tests(random-test)

add_executable(perlin-test perlin-test.cc)
target_link_libraries(perlin-test PRIVATE GTest::gtest_main lightrand)
gtest_discover_tests(perlin-test)

add_executable(simplex-test simplex-test.cc)
target_link_libraries(simplex-test PRIVATE GTest::gtest_main lightrand)
gtest_discover_tests(simplex-test)
