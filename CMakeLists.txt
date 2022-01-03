cmake_minimum_required (VERSION 3.18)
project (measure LANGUAGES CXX)

include_directories (include/)
set (CMAKE_CXX_STANDARD 17)

add_executable (01-lcg-random-no-unroll src/01-lcg-random.cpp include/measure.h)
target_compile_options (01-lcg-random-no-unroll PUBLIC "-fno-unroll-loops")
add_executable (01-lcg-random-unroll src/01-lcg-random.cpp include/measure.h)
target_compile_options (01-lcg-random-unroll PUBLIC "-funroll-loops")
