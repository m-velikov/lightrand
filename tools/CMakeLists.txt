add_executable(gen-ziggurat-table gen-ziggurat-table.cc)
target_compile_features(gen-ziggurat-table PRIVATE cxx_std_20)

add_executable(gen-bits gen-bits.cc)
target_link_libraries(gen-bits PRIVATE argparse::argparse lightrand)

add_executable(gen-uniform gen-uniform.cc)
target_link_libraries(gen-uniform PRIVATE argparse::argparse lightrand)

add_executable(gen-normal gen-normal.cc)
target_link_libraries(gen-normal PRIVATE argparse::argparse lightrand)
