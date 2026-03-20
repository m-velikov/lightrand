from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps, cmake_layout
from conan.tools.build import check_min_cppstd
from os import path
from pathlib import Path


class Lightrand(ConanFile):
    name = "lightrand"
    version = "0.1"
    license = "TBD"
    author = "Momchil Velikov <momchil.velikov@gmail.com>"
    url = "https://github.com/momchil-velikov"
    description = "A pseudo-random number generator library"

    settings = "os", "compiler", "build_type", "arch"
    user = "chill"
    channel = "stable"
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    requires = ("gtest/1.17.0", "benchmark/1.9.4")

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

    # def package_id(self):
    #     self.info.settings.compiler.libcxx = 'not used'
