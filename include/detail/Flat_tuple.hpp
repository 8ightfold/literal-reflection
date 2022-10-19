#ifndef STATIC_REFLECTION_DETAIL_FLAT_TUPLE_HPP
#define STATIC_REFLECTION_DETAIL_FLAT_TUPLE_HPP

#include <type_traits>
#include <utility>

namespace rflct::detail {
    template <std::size_t N>
    using Integral = std::integral_constant<std::size_t, N>;

    template <std::size_t N>
    using Seq = decltype(std::make_index_sequence<N>());


    namespace detail {
        template <bool C, typename T, typename U>
        struct If {
            using type = T;
        };

        template <typename T, typename U>
        struct If <false, T, U> {
            using type = U;
        };
    }

    template <bool C, typename T, typename U>
    using If = typename detail::If<C, T, U>::type;

    namespace detail {
        template <typename T>
        struct type_wrapper {
            using type = T;
        };
    }

    class Tuple_impl {
        template <typename T, std::size_t N>
        struct tuple_element {
        private:
            T data;
            using Ret = std::add_lvalue_reference_t<typename std::remove_reference_t<T>>;

        protected:
            constexpr Ret retrieve(Integral<N>) {
                return data;
            }

            static constexpr auto retrieve_wrapped(Integral<N>) {
                return detail::type_wrapper<T>{};
            }

        public:
            constexpr tuple_element(T& data)  : data(data) {}
            constexpr tuple_element(T&& data) : data(data) {}
        };

        template <typename...UU>
        struct tuple_elements : public UU...
        {
            using self_t = tuple_elements<UU...>;
            using UU::retrieve...;
            using UU::retrieve_wrapped...;

            static constexpr std::size_t count = sizeof...(UU);

            template <std::size_t N>
            constexpr decltype(auto) get_value(Integral<N>) {
                static_assert(N < count, "Out of bounds index.");
                return retrieve(Integral<N>{});
            }

            template <std::size_t N>
            static constexpr auto get_type(Integral<N>) {
                static_assert(N < count, "Out of bounds index.");
                return self_t::retrieve_wrapped(Integral<N>{});
            }
        };

        template <typename, typename...> struct pack_members;

        template <std::size_t...NN, typename...UU>
        struct pack_members <std::index_sequence<NN...>, UU...>
        {
            using type = tuple_elements<tuple_element<UU, NN>...>;
        };

    public:
        template <typename...UU>
        using tuple_t = typename pack_members<Seq<sizeof...(UU)>, UU...>::type;
    };

    template <typename...TT>
    class Flat_tuple : Tuple_impl::tuple_t<TT...>
    {
        using inherited_t = Tuple_impl::tuple_t<TT...>;

        template <bool, std::size_t>
        struct type_at_impl {
            using type = void;
        };

        template <std::size_t N>
        struct type_at_impl <true, N> {
            using type = typename decltype(inherited_t::get_type(Integral<N>{}))::type;
        };

    public:
        Flat_tuple(TT&&...tt) : inherited_t { std::forward<TT>(tt)... } {}

        template <std::size_t N>
        using type_at = typename type_at_impl<(N < sizeof...(TT)), N>::type;

        template <std::size_t N>
        constexpr decltype(auto) get(Integral<N> i) {
            static_assert(N < inherited_t::count, "Out of bounds index.");
            return this->get_value(i);
        }

        template <std::size_t N>
        constexpr decltype(auto) get() {
            static_assert(N < inherited_t::count, "Out of bounds index.");
            return this->get_value(Integral<N>{});
        }

        template <std::size_t N>
        constexpr decltype(auto) operator[](Integral<N> i) {
            static_assert(N < inherited_t::count, "Out of bounds index.");
            return this->get_value(i);
        }
    };
}

template <std::size_t N>
constexpr auto I = Integral<N>{};

#endif //STATIC_REFLECTION_DETAIL_FLAT_TUPLE_HPP
