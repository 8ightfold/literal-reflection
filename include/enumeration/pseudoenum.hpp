#ifndef LITERAL_TYPE_HPP_PSEUDOENUM_HPP
#define LITERAL_TYPE_HPP_PSEUDOENUM_HPP

#include "../util/tuple.hpp"

namespace rflct {
    template <typename /* sequence */, typename...> struct Pseudoenum;

    template <std::size_t...NN, typename...TT>
    struct Pseudoenum <std::index_sequence<NN...>, TT...> {
        static constexpr util::Tuple<std::index_sequence<NN...>, TT...> data {};
    };
}

#endif //LITERAL_TYPE_HPP_PSEUDOENUM_HPP
