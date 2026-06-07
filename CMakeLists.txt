cmake_minimum_required(VERSION 3.28)

project(lightrand LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set baseline to x86-64-v3 (AVX2) globally for x86_64 architectures
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    if(MSVC)
        add_compile_options(/arch:AVX2)
    else()
        add_compile_options(-march=x86-64-v3)
    endif()
endif()

# Determine where to copy compile_commands.json so the IDE can find it.
# If the build directory is a subdirectory of 'build', copy it to the base 'build' folder.
# Otherwise, copy it to the source root.
string(FIND "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/build" _pos)
if(_pos EQUAL 0)
    set(_comp_db_dest "${CMAKE_SOURCE_DIR}/build")
else()
    set(_comp_db_dest "${CMAKE_SOURCE_DIR}")
endif()

if(NOT CMAKE_BINARY_DIR STREQUAL _comp_db_dest)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_comp_db_dest}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${_comp_db_dest}/compile_commands.json"
        COMMENT "Copying compile_commands.json to ${_comp_db_dest}"
    )
endif()

if(MSVC)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--extra-arg=/EHsc;--extra-arg=-D_WIN32")
else()
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()
find_package(benchmark REQUIRED)
find_package(argparse REQUIRED)

include(CTest)

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(benchmarks)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

find_program(CPPCHECK_CMD cppcheck)
if(CPPCHECK_CMD)
    add_custom_target(run-cppcheck
        COMMAND ${CPPCHECK_CMD} --enable=all --library=googletest --check-level=exhaustive --suppress=missingIncludeSystem --suppress=constParameterCallback --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=functionStatic --inline-suppr --language=c++ -I ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks
        ${CMAKE_CURRENT_SOURCE_DIR}/tools
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cppcheck..."
    )
endif()
