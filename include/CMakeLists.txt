cmake_minimum_required(VERSION 3.23)
project(static_reflection)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(LITERAL_POWER=128)
add_compile_definitions(ENUM_REFLECTION_RANGE=128)

set(DETAIL detail/compiler_macros.h detail/extract_typename.hpp detail/literal_type.hpp detail/preprocessor_fold.hpp)
set(UTIL util/tuple.hpp util/passert.h util/type_list.hpp util/type_pair.hpp util/stypeid.hpp)
set(ENUMERATION enumeration/enum_reflection.hpp enumeration/pseudoenum.hpp)
set(PARSING parsing/lexer.hpp parsing/character_traits.hpp util/short_types.hpp parsing/token.hpp)

add_executable(static_reflection main.cpp ${DETAIL} ${UTIL} ${ENUMERATION} ${PARSING})
