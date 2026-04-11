from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps, cmake_layout
from conan.tools.build import check_min_cppstd
from os import path
from pathlib import Path


class Lightrand(ConanFile):
    name = "lightrand"
    version = "0.1"
    license = "MIT"
    author = "Momchil Velikov <momchil.velikov@gmail.com>"
    url = "https://github.com/m-velikov/lightrand"
    description = "A modern C++20 library for fast, high-quality pseudo-random number generation"

    settings = "os", "compiler", "build_type", "arch"
    user = "chill"
    channel = "stable"
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def build_requirements(self):
        self.test_requires("gtest/1.17.0")
        self.test_requires("benchmark/1.9.4")
        self.test_requires("argparse/3.2")

    def validate(self):
        check_min_cppstd(self, "20")

    def layout(self):
        cmake_layout(self, src_folder=".", build_folder="build")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["lightrand"]
        self.cpp_info.set_property("cmake_target_name", "lightrand::lightrand")
