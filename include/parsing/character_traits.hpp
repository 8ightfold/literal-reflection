#ifndef LITERAL_TYPE_HPP_CHARACTER_TRAITS_HPP
#define LITERAL_TYPE_HPP_CHARACTER_TRAITS_HPP

namespace rflct {
    template <char C>
    constexpr bool is_number = (C >= '0') && (C <= '9');

    template <char C>
    constexpr bool is_letter = ((C >= 'a') && (C <= 'z')) || ((C >= 'A') && (C <= 'Z'));

    template <char C>
    constexpr bool is_whitespace = (C == ' ') || (C == '\t') || (C == 0xA) || (C == 0xB) || (C == '\n');

    template <char C>
    constexpr bool is_eof = (C == '\0');

    template <char C>
    constexpr bool is_symbol =
            ((C >= '!') && (C <= '/')) || ((C >= ':') && (C <= '@')) ||
            ((C >= '[') && (C <= '`')) || ((C >= '{') && (C <= '~'));

    template <char C>
    constexpr bool is_non_ws = is_number<C> || is_letter<C> || is_symbol<C>;
}

#endif //LITERAL_TYPE_HPP_CHARACTER_TRAITS_HPP
