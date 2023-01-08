#ifndef LITERAL_TYPE_HPP_TYPE_PAIR_HPP
#define LITERAL_TYPE_HPP_TYPE_PAIR_HPP

namespace rflct::util {
    template <typename T, typename U>
    struct Type_pair {
        using first = T;
        using second = U;
    };

    template <auto T, auto U>
    struct Value_pair {
        static constexpr decltype(T) first = T;
        static constexpr decltype(U) second = U;
    };
}

#endif //LITERAL_TYPE_HPP_TYPE_PAIR_HPP
