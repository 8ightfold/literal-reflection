#ifndef LITERAL_TYPE_HPP_TOKEN_HPP
#define LITERAL_TYPE_HPP_TOKEN_HPP

#include "../detail/literal_type.hpp"
#include "../util/short_types.hpp"
#include "character_traits.hpp"

namespace rflct {
    template <std::size_t N, char...CC>
    struct Token {
        using type = Char_pack<CC...>;
        static constexpr char const data[sizeof...(CC)] { CC... };
        static constexpr std::size_t offset = N;
        static constexpr std::size_t count = sizeof...(CC);
    };

    struct Eof_t {};
    struct Empty_t {};

    template <std::size_t N, char...CC>
    constexpr std::size_t read_to_ws(Char_pack<CC...> pack) {
        using pack_t = Char_pack<CC...>;
        if constexpr(is_whitespace<pack_t::data[N]>) return N;
        else if constexpr(N >= sizeof...(CC)) return max_str_len;
        else return read_to_ws<N + 1>(pack);
    }

    template <std::size_t N, char...CC>
    constexpr auto read_next(Char_pack<CC...> pack) {
        using pack_t = Char_pack<CC...>;
        if constexpr(N >= sizeof...(CC)) return Eof_t{};
        else if constexpr(is_whitespace<pack_t::data[N]>) {
            return read_next<N + 1>(pack);
        }
        else {
            constexpr std::size_t begin = N;
            constexpr std::size_t end = read_to_ws<N>(pack) - N;
            if constexpr(end + N == max_str_len) return Eof_t{};
            else {
                return [&] <std::size_t...NN> (std::index_sequence<NN...>) {
                    return Token<begin, pack_t::data[begin + NN]...>{};
                } (Seq<end>{});
            }
        }
    }

    template <typename Str, std::size_t Offset>
    using next_token = decltype(read_next<Offset>( Str{} ));
}

#endif //LITERAL_TYPE_HPP_TOKEN_HPP
