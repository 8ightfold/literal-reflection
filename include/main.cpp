#include "parsing/token.hpp"
#include "util/type_list.hpp"
#include "util/type_pair.hpp"
#include "detail/extract_typename.hpp"

namespace rflct {
    template <std::size_t Line>
    struct Do_fail {
        Do_fail() { static_assert(std::is_same_v<void, void>, "Failed to read string."); }
    };

#   define $FAIL rflct::Do_fail<__LINE__>
#   define $cmp(T, str) std::is_same_v<T, rflct::remove_terminator_t< $( str ) >>

    enum class reflected_type {
        Enum, Obj
    };

    template <reflected_type, typename, typename...>
    struct Object;

    template <reflected_type T, char...CC, typename...Members>
    struct Object <T, Char_pack<CC...>, util::Type_list<Members...>> {
        static constexpr reflected_type type = T;
        using name = Char_pack<CC...>;
        using members = util::Type_list<Members...>;
    };



    template <typename S>
    constexpr auto get_brackets() {
        constexpr std::string_view str { S::data };
        constexpr std::size_t front = str.find('{');
        constexpr std::size_t back = str.rfind('}');
        static_assert(front != std::string_view::npos, "Opening bracket could not be located.");
        static_assert(back != std::string_view::npos, "Closing bracket could not be located.");
        return std::pair{ front, back };
    }

    template <std::size_t Off, char...CC>
    constexpr auto read_enum_members(Char_pack<CC...> pack) {

    }

    namespace detail {
        template <typename S, std::size_t N>
        constexpr std::size_t forward_find_text() {
            if constexpr(is_non_ws<S::data[N]>) return N;
            else return forward_find_text<S, N + 1>();
        }

        template <typename S, std::size_t N>
        constexpr std::size_t reverse_find_text() {
            if constexpr(is_non_ws<S::data[N]>) return N;
            else return reverse_find_text<S, N - 1>();
        }

        template <typename S>
        constexpr auto clip_member_list() {
            constexpr std::size_t front = get_brackets<S>().first, back = get_brackets<S>().second;
            constexpr std::size_t fw = forward_find_text<S, front + 1>(), bw = reverse_find_text<S, back - 1>();
            if constexpr(fw == back && bw == front) return Empty_t{};
            else {
                using sub_t = substr_t<S, fw, bw - fw>;
                using ret_t = remove_terminator_t<sub_t>;
                return ret_t{};
            }
        }
    }

    template <typename S>
    using clip_members = decltype(detail::clip_member_list<S>());

    template <typename S, char...CCname>
    constexpr auto read_enum(Char_pack<CCname...>) {
        constexpr reflected_type type = reflected_type::Enum;
        using name = Char_pack<CCname...>;
        using members_t = clip_members<S>;
        if constexpr(std::is_same_v<Empty_t, members_t>) {
            using object_t = Object<type, name, util::Type_list<>>;
            return object_t{};
        }
        else {
            using member_list = decltype(read_enum_members<0>(members_t{}));
            using object_t = Object<type, name, util::Type_list<members_t>>;
            return object_t{};
        }
    }

    template <typename S>
    constexpr auto begin_read() {
        using type_t = next_token<S, 0>;
        using type_t_name = typename type_t::type;
        if constexpr(std::is_same_v<type_t, Eof_t>) return Empty_t{};
        else if constexpr($cmp(type_t_name, "enum")) {
            using name_t = next_token<S, type_t::offset + type_t::count>;
            using nulled_name_t = add_terminator_t<typename name_t::type>;
            return read_enum<S>(nulled_name_t{});
        }
        else if constexpr($cmp(type_t_name, "obj")) {
            using name_t = next_token<S, type_t::offset + type_t::count>;
        }
        else return $FAIL{};
    }

    template <typename S>
    using lex_t = decltype(begin_read<S>());
}


////////////////////////////////////////////////////////////////////////////////
using enumerator = $(R"rf(
    enum test {
        zero, one, two, four = 4
    }
)rf");

using object = $(R"rf(
    obj Foo {
        int member;
        float other_member;
    }
)rf");

//using lexed = rflct::lex_t<enumerator>;
#include <iostream>
#include "util/stypeid.hpp"

constexpr auto print_token = [] <std::size_t N, char...CC> (rflct::Token<N, CC...>) {
    std::cout << "{\n";
    std::cout << "\tLength: " << sizeof...(CC) << '\n';
    std::cout << "\tValue: "; ((std::cout << CC), ...); std::cout << '\n';
    std::cout << "\tOffset: " << N << '\n';
    std::cout << "\tType: " << stypeid(rflct::Token<N, CC...>{}) << "\n}\n";
};
constexpr auto print_pack = [] <char...CC> (rflct::Char_pack<CC...>) {
    std::cout << "{\n";
    std::cout << "\tLength: " << sizeof...(CC) << '\n';
    std::cout << "\tValue: "; ((std::cout << CC), ...); std::cout << '\n';
    std::cout << "\tType: " << stypeid(rflct::Char_pack<CC...>{}) << "\n}\n";
};
constexpr auto print_object = [] <rflct::reflected_type T, char...CC, typename...TT>
(rflct::Object<T, rflct::Char_pack<CC...>, rflct::util::Type_list<TT...>>) {
    std::cout << "{\n";
    std::cout << "\tType: " << $reflect_auto( T )::data << '\n';
    std::cout << "\tName: " << rflct::Char_pack<CC...>::data << '\n';
    //std::cout << "\tMembers: " << stypeid(rflct::util::Type_list<TT...>{}) << '\n';
    std::cout << "}\n";
};

int main() {
    using lexed = rflct::lex_t<enumerator>;
    print_object(lexed{});
}
