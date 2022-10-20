#ifndef STATIC_REFLECTION_ENUM_REFLECTION_HPP
#define STATIC_REFLECTION_ENUM_REFLECTION_HPP

#include "../detail/extract_typename.hpp"

#ifndef ENUM_REFLECTION_RANGE
#  define ENUM_REFLECTION_RANGE 8192
#endif

#include <array>

namespace rflct {
    template <typename E, std::size_t N>
    constexpr bool is_valid_enum_impl() {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        using utype = std::underlying_type_t<E>;
        constexpr E val = static_cast<E>( static_cast<utype>(N) );
        using val_str = $reflect_auto(val);
        using find_str = concat_t<type_name_t<E>, $( "::" )>;
        return begins_with_v<val_str, find_str>;
    };

    template <typename E, std::size_t N>
    constexpr bool is_valid_enum_v = is_valid_enum_impl<E, N>();

    template <typename E, std::size_t...NN>
    constexpr auto valid_sequence_impl(std::index_sequence<NN...>) {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        return std::array{ is_valid_enum_v<E, NN>... };
    }

    template <typename E>
    struct Valid_sequence {
        static constexpr auto value = valid_sequence_impl<E>(std::make_index_sequence<ENUM_REFLECTION_RANGE>());
    };

    template <typename E>
    constexpr auto& valid_sequence_v = Valid_sequence<E>::value;

    template <typename E>
    constexpr std::size_t enum_count_impl() {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        std::size_t count = 0;
        for(auto& b : valid_sequence_v<E>) {
            count = count + static_cast<std::size_t>(b);
        }
        return count;
    }

    template <typename E>
    constexpr std::size_t enum_count_v = enum_count_impl<E>();

    template <typename E>
    constexpr auto enum_sequence_impl() {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        constexpr auto& valid = valid_sequence_v<E>;
        constexpr std::size_t count = enum_count_v<E>;
        std::array<E, count> enums{};
        std::size_t array_idx = 0;

        for(std::size_t idx = 0; array_idx < count; ++idx) {
            if(valid[idx]) {
                enums[array_idx] = static_cast<E>(idx);
                ++array_idx;
            }
        }
        return enums;
    }

    template <typename E>
    constexpr auto enum_sequence_v = enum_sequence_impl<E>();

    template <typename E, std::size_t...NN>
    constexpr auto reflect_enum_impl(std::index_sequence<NN...>) {
        constexpr auto& values = enum_sequence_v<E>;
        return std::array { std::string_view{ $manual_reflect_auto(E, values[NN])::data }... };
    }

    template <typename E>
    constexpr auto reflect_enum() {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        constexpr auto& values = enum_sequence_v<E>;
        return reflect_enum_impl<E>(std::make_index_sequence<values.size()>());
    }

    template <typename E>
    constexpr auto reflect_enum_v = reflect_enum<E>();

    template <typename E>
    constexpr std::size_t enum_max() {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        constexpr auto& enums = enum_sequence_v<E>;
        return static_cast<std::size_t>( enums[enums.size() - 1] ) + 1;
    }

    template <typename E, std::size_t N>
    constexpr std::string_view reflect_enum_value_or_empty() {
        if constexpr(is_valid_enum_v<E, N>) {
            return $reflect_auto(static_cast<E>(N))::data;
        }
        else return "" ;
    }

    template <typename E, std::size_t...NN>
    constexpr auto quick_reflect_enum_impl(std::index_sequence<NN...>) {
        static_assert(std::is_enum_v<E>, "Must be used with an enum.");
        return std::array<std::string_view, sizeof...(NN)> { reflect_enum_value_or_empty<E, NN>()... };
    }

    template <typename E>
    constexpr auto quick_reflect_enum() {
        constexpr std::size_t count = enum_max<E>();
        return quick_reflect_enum_impl<E>(std::make_index_sequence<count>());
    }

    template <typename E>
    constexpr auto quick_reflect_enum_v = quick_reflect_enum<E>();

    namespace enum_literals {
        template <typename E, std::enable_if_t<std::is_enum_v<E>, bool> = true>
        std::ostream& operator<<(std::ostream& os, const E& e) {
            constexpr auto& enums = rflct::quick_reflect_enum_v<E>;
            return os << enums[static_cast<std::size_t>(e)];
        }
    }
}

/*
#define REFLECT(type)                                                        \
do {                                                                        \
    std::cout << "Reflecting " << rflct::type_name_t<type>::data << ": \n";   \
        for(auto& s : rflct::reflect_enum_v<type>) {                          \
        std::cout << s << ' ';                                              \
    }                                                                       \
    std::cout << '\n';                                                      \
} while(0)
 */

#endif //STATIC_REFLECTION_ENUM_REFLECTION_HPP
