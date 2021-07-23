cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)

add_executable(result result.cpp ../src/sjdlib.h)
add_compile_options(-Wall -Wextra -pedantic -Werror)
