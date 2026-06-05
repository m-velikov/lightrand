add_library(lightrand STATIC random.cc ziggurat-data.cc)

target_include_directories(lightrand PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(lightrand PUBLIC cxx_std_20)
set_target_properties(lightrand PROPERTIES CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

install(TARGETS lightrand
    EXPORT lightrand-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ../include/
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
