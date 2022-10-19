#ifndef STATIC_REFLECTION_EXTRACT_TYPENAME_HPP
#define STATIC_REFLECTION_EXTRACT_TYPENAME_HPP

#include "compiler_macros.h"

#if defined(COMPILER_GCC)
#  define $func_name __PRETTY_FUNCTION__
#  define $pretty_front "constexpr auto rflct::type_name_impl() [with T = "
#  define $pretty_back "]"
#elif defined(COMPILER_CLANG)
#  define $func_name __PRETTY_FUNCTION__
#  define $pretty_front "auto rflct::type_name_impl() [T = "
#  define $pretty_back "]"
#elif defined(COMPILER_MSVC)
#  define $func_name __FUNCSIG__
#  define $pretty_front "auto __cdecl rflct::type_name_impl<"
#  define $pretty_back ">(void)"
#elif defined(COMPILER_ICC)
#  define $func_name __PRETTY_FUNCTION__
#  define $pretty_front "constexpr auto rflct::type_name_impl() [with T = "
#  define $pretty_back "]"
#else
#  define $func_name "<null>"
#  define $pretty_front "<"
#  define $pretty_back ">"
#endif

#include "literal_type.hpp"

namespace rflct {
    template <typename T>
    constexpr auto type_name_impl() {
        constexpr std::size_t Pos = sizeof($pretty_front) - 1;
        constexpr std::size_t Back_v = sizeof($pretty_back) - 1;
        constexpr std::size_t Count = sizeof($func_name) - Pos - Back_v - 1;
        using pretty = $post( $func_name );
        using name = substr_t<pretty, Pos, Count>;
        return name{};
    }

    template <typename T>
    class Type_name {
        static constexpr auto value = type_name_impl<T>();
    public:
        using type = typename std::remove_cv_t<decltype(value)>;
    };

    template <typename T>
    using type_name_t = typename Type_name<T>::type;
}

#endif //STATIC_REFLECTION_EXTRACT_TYPENAME_HPP
