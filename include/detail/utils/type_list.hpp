#ifndef LITERAL_TYPE_HPP_TYPE_LIST_HPP
#define LITERAL_TYPE_HPP_TYPE_LIST_HPP

#include <cstddef>
#include <limits>
#include <type_traits>

namespace rflct {
    template <typename Ty> struct Type_wrapper { using type = Ty; };

    template <typename...TT>
    class Type_list
    {
        template <size_t I = 0, size_t N, size_t C,
                typename Ty, typename...UU>
        static consteval inline auto get_at() {
            static_assert(N < C, "Out of bounds");
            if constexpr(I == N) return Type_wrapper<Ty>{};
            else return get_at<I + 1, N, C, UU...>();
        };

        template <typename U, size_t I = 0, size_t C>
        static consteval inline size_t locate_impl() {
            return std::numeric_limits<size_t>::max();
        };

        template <typename U, size_t I = 0, size_t C,
                typename T, typename...UU>
        static consteval inline size_t locate_impl() {
            if constexpr (std::is_same_v<U, T>) return I;
            else return locate_impl<U, I + 1, C, UU...>();
        };

        static inline constexpr size_t count_impl = sizeof...(TT);

    public:
        static inline consteval size_t count()
        { return count_impl; };

        template <typename...UU> struct append
        { using type = Type_list <TT..., UU...>; };

        template <typename...UU> struct prepend
        { using type = Type_list <UU..., TT...>; };

        template <size_t N>
        static inline consteval auto get()
        {
            return get_at<0, N, Type_list<TT...>::count_impl, TT...>();
        };

        template <typename Ty>
        static inline consteval bool exists()
        {
            return locate_impl<Ty, 0, Type_list<TT...>::count_impl, TT...>()
                   != std::numeric_limits<size_t>::max();
        };

        template <typename Ty>
        static inline consteval size_t locate()
        {
            return locate_impl<Ty, 0, Type_list<TT...>::count_impl, TT...>();
        };

        template <bool B, typename...UU>
        struct append_if
        { using type = Type_list <TT..., UU...>; };

        template <typename...UU>
        struct append_if <false, UU...>
        { using type = Type_list <TT...>; };
    };

    template <> class Type_list<>
    {
    public:
        static inline consteval size_t count() { return 0; };

        template <typename Ty>
        static inline consteval bool exists() { return false; };

        template <typename Ty>
        static inline consteval size_t locate()
        { return std::numeric_limits<size_t>::max(); };

        template <typename...UU> struct append
        { using type = Type_list <UU...>; };

        template <typename...UU> struct prepend
        { using type = Type_list <UU...>; };

        template <bool B, typename...UU>
        struct append_if
        { using type = Type_list <UU...>; };

        template <typename...UU>
        struct append_if <false, UU...>
        { using type = Type_list <>; };
    };

    template <typename List, typename T, typename...TT>
    constexpr inline auto tl_flip(rflct::Type_list<T, TT...>)
    {
        return tl_flip<typename List::template prepend<T>::type>(rflct::Type_list<TT...>{});
    };

    template <typename List>
    consteval inline auto tl_flip(rflct::Type_list<>) { return List{}; };
}

#endif //LITERAL_TYPE_HPP_TYPE_LIST_HPP
