cmake_minimum_required(VERSION 3.23)
project(literal_reflection)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(LITERAL_POWER=128)

add_executable(literal_reflection literal_type.hpp passert.h compiler_macros.h extract_typename.hpp)
