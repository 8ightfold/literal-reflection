#ifndef LITERAL_TYPE_HPP_SHORT_TYPES_HPP
#define LITERAL_TYPE_HPP_SHORT_TYPES_HPP

#include <cstddef>
#include <utility>

namespace rflct {
    template <std::size_t N>
    using Integral = std::integral_constant<std::size_t, N>;

    template <typename T>
    struct Type {
        using type = T;
    };

    template <std::size_t N>
    using Seq = decltype(std::make_index_sequence<N>());
}

template <std::size_t N>
constexpr auto I = rflct::Integral<N>{};

template <typename U>
constexpr auto T = rflct::Type<U>{};

#endif //LITERAL_TYPE_HPP_SHORT_TYPES_HPP
