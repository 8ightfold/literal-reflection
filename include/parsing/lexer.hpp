#ifndef LITERAL_TYPE_HPP_LEXER_HPP
#define LITERAL_TYPE_HPP_LEXER_HPP

#include "enumeration/enum_reflection.hpp"
#include "util/tuple.hpp"
#include "util/type_list.hpp"
#include "util/type_pair.hpp"

namespace rflct {
    template <char C>
    constexpr bool is_number = (C >= '0') && (C <= '9');

    template <char C>
    constexpr bool is_letter = ((C >= 'a') && (C <= 'z')) || ((C >= 'A') && (C <= 'Z'));

    template <char C>
    constexpr bool is_whitespace = (C == ' ') || (C == '\t') || (C == 0xB) || (C == '\n');

    template <char C>
    constexpr bool is_eof = (C == '\0');

    template <char C>
    constexpr bool is_symbol =
            ((C >= '!') && (C <= '/')) || ((C >= ':') && (C <= '@')) ||
            ((C >= '[') && (C <= '`')) || ((C >= '{') && (C <= '~'));

    template <char C>
    constexpr bool is_non_ws = is_number<C> || is_letter<C> || is_symbol<C>;
}

namespace rflct {
    template <std::size_t Line>
    struct Do_fail {
        Do_fail() { static_assert(std::is_same_v<void, void>, "Failed to read string."); }
    };

    enum class lexer_state {
        discarding,
        between,
        reading
    };

    enum class internal {
        type, name, members
    };

    enum class object_type {
        null, enumerator, obj
    };

    template <object_type, typename /* name */, typename /* items */> struct Object_container {};

    template <object_type O, char...CC, typename...TT>
    struct Object_container<O, Char_pack<CC...>, util::Type_list<TT...>> {
    static constexpr object_type object = O;
    using name = Char_pack<CC...>;
    using member_list = util::Type_list<TT...>;
};

template <lexer_state> struct Lexer {};
template <internal> struct I_state {};

template <typename S, std::size_t I, char...CC>
constexpr auto read_until_ws(Char_pack<CC...>) {
    constexpr char C = S::data[I];

    if constexpr(is_whitespace<C>) {
        return util::Type_pair<Char_pack<CC..., '\0'>, Integral<I>>{};
    }
    else {
        using next_t = Char_pack<CC..., C>;
        return read_until_ws<S, I + 1>(next_t{});
    }
}


template <typename S, std::size_t I, std::size_t Pcount = 0, object_type O, char...CC, typename...TT>
constexpr auto read(
        Object_container<O, Char_pack<CC...>, util::Type_list<TT...>> obj,
        Lexer<lexer_state::discarding> lex
) {
    constexpr char C = S::data[I];

    if constexpr(is_eof<C>) return obj;
    else if constexpr(is_whitespace<C>) {
        return read<S, I + 1, Pcount>(obj, lex);
    }
    else if constexpr(is_non_ws<C>) {
        if constexpr(O == object_type::null) {
            return read<S, I, Pcount>(obj, Lexer<lexer_state::reading>{}, I_state<internal::type>{});
        }
        else if constexpr(sizeof...(CC) == 0) {
            return read<S, I, Pcount>(obj, Lexer<lexer_state::reading>{}, I_state<internal::name>{});
        }
        else {
            return obj;
            //return read<S, I, Pcount>(obj, Lexer<lexer_state::reading>{}, I_state<internal::members>{});
        }
    }
    else {
        return Do_fail<__LINE__>{};
    }
}

template <typename S, std::size_t I, std::size_t Pcount = 0, object_type O, char...CC, typename...TT>
constexpr auto read(
        Object_container<O, Char_pack<CC...>, util::Type_list<TT...>> obj,
        Lexer<lexer_state::between> lex
) {
    constexpr char C = S::data[I];

    if constexpr(C == '}') {
        if constexpr(Pcount - 1 == 0) {
            return read<S, I - 1, Pcount + 1>(obj, Lexer<lexer_state::discarding>{});
        }
        else return read<S, I - 1, Pcount - 1>(obj, Lexer<lexer_state::between>{});
    }
    else return read<S, I - 1, Pcount>(obj, Lexer<lexer_state::between>{});
}

template <typename S, std::size_t I, std::size_t Pcount = 0, object_type O, char...CC, typename...TT, internal IS>
constexpr auto read(
        Object_container<O, Char_pack<CC...>, util::Type_list<TT...>> obj,
        Lexer<lexer_state::reading> lex, I_state<IS> istate
) {
    constexpr char C = S::data[I];

    if constexpr(std::is_same_v<I_state<IS>, I_state<internal::type>>) {
        using name_val_pair = decltype(read_until_ws<S, I>(Char_pack<>{}));
        using name = typename name_val_pair::first;
        constexpr std::size_t next_idx = name_val_pair::second::value;

        if constexpr(std::is_same_v<name, $("enum")>) {
            using obj_t = Object_container<object_type::enumerator, Char_pack<CC...>, util::Type_list<TT...>>;
            return read<S, next_idx, Pcount>(obj_t{}, Lexer<lexer_state::discarding>{});
        }
        else if constexpr(std::is_same_v<name, $("obj")>) {
            using obj_t = Object_container<object_type::obj, Char_pack<CC...>, util::Type_list<TT...>>;
            return read<S, next_idx, Pcount>(obj_t{}, Lexer<lexer_state::discarding>{});
        }
        else {
            return Do_fail<__LINE__>{};
        }
    }
    else if constexpr(std::is_same_v<I_state<IS>, I_state<internal::name>>) {
        using name_val_pair = decltype(read_until_ws<S, I>(Char_pack<>{}));
        using name = typename name_val_pair::first;
        constexpr std::size_t next_idx = name_val_pair::second::value;

        using obj_t = Object_container<object_type::enumerator, name, util::Type_list<TT...>>;
        return read<S, next_idx>(obj_t{}, Lexer<lexer_state::discarding>{});
    }
    else {
        if constexpr(C == '{') {
            return read<S, I + 1, Pcount + 1>(obj, Lexer<lexer_state::between>{});
        }
        else {
            return read<S, I + 1, Pcount>(obj, lex);
        }
    }
}

template <typename S>
constexpr auto do_read() {
    static_assert(is_char_pack_v<S>, "Must be used with a char pack.");
    using default_t = Object_container<object_type::null, Char_pack<>, util::Type_list<>>;
    return read<S, 0>(default_t{}, Lexer<lexer_state::discarding>{});
}

template <typename S>
using lex_t = decltype(do_read<S>());
}

#endif //LITERAL_TYPE_HPP_LEXER_HPP
