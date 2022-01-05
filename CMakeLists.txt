cmake_minimum_required (VERSION 3.18)
project (measure LANGUAGES CXX)

include_directories (include/)
set (CMAKE_CXX_STANDARD 17)

add_executable (01-lcg-random-no-unroll src/01-lcg-random.cpp include/measure.h)
target_compile_options (01-lcg-random-no-unroll PUBLIC "-fno-unroll-loops")
add_executable (01-lcg-random-unroll src/01-lcg-random.cpp include/measure.h)
target_compile_options (01-lcg-random-unroll PUBLIC "-funroll-loops")

add_executable (02-lcg-random-mod src/02-lcg-random-mod.cpp include/measure.h)
target_compile_options (02-lcg-random-mod PUBLIC "-fno-unroll-loops")
