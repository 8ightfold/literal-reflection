#ifndef STATIC_REFLECTION_EXTRACT_TYPENAME_HPP
#define STATIC_REFLECTION_EXTRACT_TYPENAME_HPP

#include "compiler_macros.h"

#if defined(COMPILER_GCC)
#  define $func_name __PRETTY_FUNCTION__
#  define $pretty_front "constexpr auto rflct::type_name_impl() [with T = "
#  define $pretty_back "]"
#  define $auto_pretty_front "constexpr auto rflct::auto_name_impl() [with auto V = "
#  define $sauto_pretty_middle "; T V = "
#elif defined(COMPILER_CLANG)
#  define $func_name __PRETTY_FUNCTION__
#  define $pretty_front "auto rflct::type_name_impl() [T = "
#  define $pretty_back "]"
#  define $auto_pretty_front $pretty_front
#  define $sauto_pretty_middle ", V = "
#elif defined(COMPILER_MSVC)
#  define $func_name __FUNCSIG__
#  define $pretty_front "auto __cdecl rflct::type_name_impl<"
#  define $pretty_back ">(void)"
#  define $auto_pretty_front $pretty_front
#  define $sauto_pretty_middle ","
#elif defined(COMPILER_ICC)
#  define $func_name __PRETTY_FUNCTION__
#  define $pretty_front "constexpr auto rflct::type_name_impl() [with T = "
#  define $pretty_back "]"
#  define $auto_pretty_front $pretty_front
#else
#  define $func_name "<null>"
#  define $pretty_front "<"
#  define $pretty_back ">"
#  define $auto_pretty_front $pretty_front
#endif

#include "literal_type.hpp"

namespace rflct {
    template <typename T>
    constexpr auto type_name_impl() {
        constexpr std::size_t Total_v = sizeof($func_name);
        constexpr std::size_t Pos = sizeof($pretty_front) - 1;
        constexpr std::size_t Back_v = sizeof($pretty_back) - 1;
        constexpr std::size_t Count = Total_v - Pos - Back_v - 1;
        static_assert(Count >= 0, "Underflow detected, check macros.");

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


#if COMPILER_CPP_VERSION >= 202000L && !defined(COMPILER_ICC) && false
    template <auto V>
    constexpr auto auto_name_impl() {
        constexpr std::size_t Pos = sizeof($auto_pretty_front) - 1;
        constexpr std::size_t Back_v = sizeof($pretty_back) - 1;
        constexpr std::size_t Count = sizeof($func_name) - Pos - Back_v - 1;
        using pretty = $post( $func_name );
        using name = substr_t<pretty, Pos, Count>;
        return name{};
    }

    template <auto V>
    class Auto_name {
        static constexpr auto value = auto_name_impl<V>();
    public:
        using type = typename std::remove_cv_t<decltype(value)>;
    };

    template <auto V>
    using auto_name_t = typename Auto_name<V>::type;

#   define $reflect_auto(v) rflct::auto_name_t<v>
#   define $manual_reflect_auto(T, v) rflct::auto_name_t<v>

#else
    template <typename T, T V>
    constexpr auto auto_name_impl() {
        using base_str = type_name_t<T>;
        using pretty = $post( $func_name );
        constexpr std::size_t Off = find_v<pretty, base_str>;
        constexpr std::size_t Middle_v = sizeof($sauto_pretty_middle) - 1;
        constexpr std::size_t Pos = Off + sizeof(base_str::data) - 1 + Middle_v;
        constexpr std::size_t Back_v = sizeof($pretty_back) - 1;
        constexpr std::size_t Count = sizeof($func_name) - Pos - Back_v - 1;
        using name = substr_t<pretty, Pos, Count>;
        return name{};
    }

    template <typename T, T V>
    class Auto_name {
        static constexpr auto value = auto_name_impl<T, V>();
    public:
        using type = typename std::remove_cv_t<decltype(value)>;
    };

    template <typename T, T V>
    using auto_name_t = typename Auto_name<T, V>::type;

#   define $reflect_auto(v) rflct::auto_name_t<decltype(v), v>
#   define $manual_reflect_auto(T, v) rflct::auto_name_t<T, v>

#endif
}

#endif //STATIC_REFLECTION_EXTRACT_TYPENAME_HPP
