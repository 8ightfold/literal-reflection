#ifndef LITERAL_TYPE_HPP_TYPE_PAIR_HPP
#define LITERAL_TYPE_HPP_TYPE_PAIR_HPP

namespace rflct::util {
    template <typename T, typename U>
    struct Type_pair {
        using first = T;
        using second = U;
    };
}

#endif //LITERAL_TYPE_HPP_TYPE_PAIR_HPP
