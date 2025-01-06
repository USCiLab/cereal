#ifndef CTTI_UTILITY_META_H
#define CTTI_UTILITY_META_H

#include <type_traits>
#include <cstddef>
#include <array>
#include <cstdint>

namespace ctti
{

namespace meta
{
    template<typename T>
    using decay_t = typename std::decay<T>::type;

    template<typename... Bs>
    struct assert;

    template<typename B, typename... Bs>
    struct assert<B, Bs...> : assert<Bs...>
    {
        static_assert(B::value, "Assertion failed");
    };

    template<>
    struct assert<> {};

    template<typename T>
    using void_t = typename std::conditional<sizeof(T*) >= 0, void, T>::type;

    template<typename T>
    struct identity
    {
        using type = T;
    };

    template<typename Metafunction>
    using type_t = typename Metafunction::type;

    template<bool Cond, typename T = void>
    using enable_if_t = typename std::enable_if<Cond, T>::type;

    template<typename MetafunctionClass, typename... Args>
    using apply_t = type_t<typename MetafunctionClass::template apply<Args...>>;

    template<typename MetafunctionClass, typename Seq>
    struct sequence_apply;

    template<typename MetafunctionClass,
             template<typename...> class Seq, typename... Ts>
    struct sequence_apply<MetafunctionClass, Seq<Ts...>>
    {
        using type = apply_t<MetafunctionClass, Ts...>;
    };

    template<typename MetafunctionClass, typename Seq>
    using sequence_apply_t = type_t<sequence_apply<MetafunctionClass, Seq>>;

    template<std::uint8_t I>
    using uint8_t = std::integral_constant<std::uint8_t, I>;
    template<std::uint16_t I>
    using uint16_t = std::integral_constant<std::uint16_t, I>;
    template<std::uint32_t I>
    using uint32_t = std::integral_constant<std::uint32_t, I>;
    template<std::uint64_t I>
    using uint64_t = std::integral_constant<std::uint64_t, I>;

    template<std::int8_t I>
    using int8_t = std::integral_constant<std::int8_t, I>;
    template<std::int16_t I>
    using int16_t = std::integral_constant<std::int16_t, I>;
    template<std::int32_t I>
    using int32_t = std::integral_constant<std::int32_t, I>;
    template<std::int64_t I>
    using int64_t = std::integral_constant<std::int64_t, I>;

    template<std::size_t I>
    using size_t = std::integral_constant<std::size_t, I>;

    template<bool B>
    using bool_ = std::integral_constant<bool, B>;
    using true_ = bool_<true>;
    using false_ = bool_<false>;

    template<char C>
    using char_ = std::integral_constant<char, C>;

    template<typename T, typename = void>
    struct is_integral : false_ {};
    template<typename T>
    struct is_integral<T, void_t<decltype(T::value)>> :
        true_
    {};

    template<typename T, typename = assert<is_integral<T>>>
    constexpr decltype(T::value) value()
    {
        return T::value;
    }

    template<template<typename...> class Function>
    struct defer
    {
        template<typename... Args>
        struct apply
        {
            template<typename Instance, bool = is_integral<Instance>::value>
            struct result
            {
                using type = Instance;
            };

            template<typename Instance>
            struct result<Instance, false>
            {
                using type = type_t<Instance>;
            };

            using type = type_t<result<Function<Args...>>>;
        };
    };

    struct and_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<Lhs::value && Rhs::value>;
        };
    };

    template<typename Lhs, typename Rhs>
    using and_t = apply_t<and_, Lhs, Rhs>;

    struct or_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<Lhs::value || Rhs::value>;
        };
    };

    template<typename Lhs, typename Rhs>
    using or_t = apply_t<or_, Lhs, Rhs>;

    struct add_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = std::integral_constant<decltype(Lhs::value + Rhs::value), Lhs::value + Rhs::value>;
        };
    };

    struct div_
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = std::integral_constant<decltype(Lhs::value / Rhs::value), Lhs::value / Rhs::value>;
        };
    };

    template<typename Lhs, typename Rhs>
    using add_t = apply_t<add_, Lhs, Rhs>;

    template<typename Lhs, typename Rhs>
    using div_t = apply_t<div_, Lhs, Rhs>;

    struct greater
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<(Lhs::value > Rhs::value)>;
        };
    };
    struct greater_or_equal
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<(Lhs::value >= Rhs::value)>;
        };
    };
    struct less
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<(Lhs::value < Rhs::value)>;
        };
    };
    struct less_or_equal
    {
        template<typename Lhs, typename Rhs>
        struct apply : assert<
            is_integral<Lhs>,
            is_integral<Rhs>
        >
        {
            using type = bool_<(Lhs::value <= Rhs::value)>;
        };
    };

    template<typename... Ts>
    struct list
    {
        static constexpr std::size_t size = sizeof...(Ts);
    };

    template<typename... Ts>
    constexpr std::size_t list<Ts...>::size;

    template<typename List>
    struct list_size;

    template<template<typename...> class Sequence, typename... Ts>
    struct list_size<Sequence<Ts...>> : public size_t<sizeof...(Ts)>
    {};

    template<char... Chars>
    using string = list<std::integral_constant<char, Chars>...>;


    template<typename Seq>
    struct functor
    {
        static_assert(sizeof(Seq) != sizeof(Seq), "Type is not a sequence");
    };

    template<template<typename...> class Seq, typename... Ts>
    struct functor<Seq<Ts...>>
    {
        template<typename... Us>
        struct apply
        {
            using type = Seq<Us...>;
        };

        template<template<typename...> class Seq2, typename... Us>
        struct apply<Seq2<Us...>>
        {
            using type = Seq<Us...>;
        };
    };

    template<typename Seq, typename... Ts>
    using apply_functor = apply_t<functor<Seq>, Ts...>;

    template<typename Seq, typename... Ts>
    using functor_t = apply_functor<Seq, Ts...>;

    template<typename Lhs, typename Rhs>
    struct cat;

    template<template<typename...> class Seq,
             typename... Lhs, typename... Rhs>
    struct cat<Seq<Lhs...>, Seq<Rhs...>>
    {
        using type = Seq<Lhs..., Rhs...>;
    };

    template<typename Lhs, typename Rhs>
    using cat_t = type_t<cat<Lhs, Rhs>>;

    template<std::size_t I>
    using index_t = std::integral_constant<std::size_t, I>;

    template<typename T, T... Values>
    using integer_sequence = list<std::integral_constant<T, Values>...>;

    template<std::size_t... Is>
    using index_sequence = list<index_t<Is>...>;

    namespace detail
    {
        template<typename Left, std::size_t Index, typename Right>
        struct split;

        template<template<typename...> class Seq, std::size_t Index,
                 typename... Left, typename Head, typename... Tail>
        struct split<Seq<Left...>, Index, Seq<Head, Tail...>>
        {
            using next = split<Seq<Left..., Head>, Index - 1, Seq<Tail...>>;

            using before = typename next::before;
            using left  = typename next::left;
            using head  = typename next::head;
            using right = typename next::right;
            using after = typename next::after;
        };

        template<template<typename...> class Seq,
            typename... Left, typename Head, typename... Tail>
        struct split<Seq<Left...>, 0, Seq<Head, Tail...>>
        {
            using before = Seq<Left...>;
            using left  = Seq<Left..., Head>;
            using head  = Head;
            using right = Seq<Head, Tail...>;
            using after = Seq<Tail...>;
        };

        template<template<typename...> class Seq,
            typename Head>
        struct split<Seq<Head>, 0, Seq<>>
        {
            using before = Seq<>;
            using left  = Seq<Head>;
            using head  = Head;
            using right = Seq<Head>;
            using after = Seq<>;
        };
    }

    template<std::size_t Index, typename... Ts>
    using pack_split = detail::split<list<>, Index, list<Ts...>>;
    template<std::size_t Index, typename... Ts>
    using pack_split_left_t = typename pack_split<Index, Ts...>::left;
    template<std::size_t Index, typename... Ts>
    using pack_get_t = typename pack_split<Index, Ts...>::head;
    template<std::size_t Index, typename... Ts>
    using pack_split_right_t = typename pack_split<Index, Ts...>::right;
    template<std::size_t Index, typename... Ts>
    using pack_split_before_t = typename pack_split<Index, Ts...>::before;
    template<std::size_t Index, typename... Ts>
    using pack_split_after_t = typename pack_split<Index, Ts...>::after;
    template<typename... Ts>
    using pack_head_t = pack_get_t<0, Ts...>;
    template<typename... Ts>
    using pack_tail_t = pack_split_after_t<0, Ts...>;

    template<std::size_t Index, typename Seq>
    struct split;
    template<std::size_t Index, template<typename...> class Seq, typename... Ts>
    struct split<Index, Seq<Ts...>>
    {
        using splitter = detail::split<Seq<>, Index, Seq<Ts...>>;

        using before = typename splitter::before;
        using left  = typename splitter::left;
        using head  = typename splitter::head;
        using right = typename splitter::right;
        using after = typename splitter::after;
    };
    template<std::size_t Index, typename Seq>
    using split_left_t = typename split<Index, Seq>::left;
    template<std::size_t Index, typename Seq>
    using get_t = typename split<Index, Seq>::head;
    template<std::size_t Index, typename Seq>
    using split_right_t = typename split<Index, Seq>::right;
    template<std::size_t Index, typename Seq>
    using split_before_t = typename split<Index, Seq>::before;
    template<std::size_t Index, typename Seq>
    using split_after_t = typename split<Index, Seq>::after;
    template<typename Seq>
    using head_t = get_t<0, Seq>;
    template<typename Seq>
    using tail_t = split_after_t<0, Seq>;


    template<typename T, typename... Ts>
    using pack_prepend_t = list<T, Ts...>;
    template<typename T, typename... Ts>
    using pack_append_t = list<Ts..., T>;
    template<typename T, std::size_t Index, typename... Ts>
    using pack_insert_t = cat_t<pack_append_t<pack_split_left_t<Index, Ts...>, T>, pack_split_right_t<Index, Ts...>>;
    template<typename T, std::size_t Index, typename... Ts>
    using pack_remove_t = cat_t<pack_split_before_t<Index, Ts...>, pack_split_after_t<Index, Ts...>>;

    template<typename T, typename Seq>
    struct prepend;
    template<typename T, template<typename...> class Seq, typename... Ts>
    struct prepend<T, Seq<Ts...>>
    {
        using type = Seq<T, Ts...>;
    };
    template<typename T, typename Seq>
    struct append;
    template<typename T, template<typename...> class Seq, typename... Ts>
    struct append<T, Seq<Ts...>>
    {
        using type = Seq<Ts..., T>;
    };

    template<typename T, typename Seq>
    using prepend_t = type_t<prepend<T, Seq>>;
    template<typename T, typename Seq>
    using append_t = type_t<append<Seq, T>>;
    template<typename T, std::size_t Index, typename Seq>
    using insert_t = cat_t<append_t<split_left_t<Index, Seq>, T>, split_right_t<Index, Seq>>;
    template<typename T, std::size_t Index, typename Seq>
    using remove_t = cat_t<split_before_t<Index, Seq>, split_after_t<Index, Seq>>;

    namespace detail
    {
        template<typename Result, typename Lhs, typename Rhs, typename Compare>
        struct merge;

        template<template<typename...> class Sequence, typename... Ts, typename LhsHead, typename... LhsTail, typename RhsHead, typename... RhsTail, typename Compare>
        struct merge<Sequence<Ts...>, Sequence<LhsHead, LhsTail...>, Sequence<RhsHead, RhsTail...>, Compare>
        {
            using next_result = typename std::conditional<
                apply_t<Compare, LhsHead, RhsHead>::value,
                Sequence<Ts..., LhsHead>,
                Sequence<Ts..., RhsHead>
            >::type;
            using next_lhs = typename std::conditional<
                apply_t<Compare, LhsHead, RhsHead>::value,
                Sequence<LhsTail...>,
                Sequence<LhsHead, LhsTail...>
            >::type;
            using next_rhs = typename std::conditional<
                apply_t<Compare, LhsHead, RhsHead>::value,
                Sequence<RhsHead, RhsTail...>,
                Sequence<RhsTail...>
            >::type;

            using type = type_t<
                merge<
                    next_result,
                    next_lhs,
                    next_rhs,
                    Compare
                >
            >;
        };

        template<template<typename...> class Sequence, typename... Ts, typename LhsHead, typename... LhsTail, typename Compare>
        struct merge<Sequence<Ts...>, Sequence<LhsHead, LhsTail...>, Sequence<>, Compare>
        {
            using type = Sequence<Ts..., LhsHead, LhsTail...>;
        };

        template<template<typename...> class Sequence, typename... Ts, typename RhsHead, typename... RhsTail, typename Compare>
        struct merge<Sequence<Ts...>, Sequence<>, Sequence<RhsHead, RhsTail...>, Compare>
        {
            using type = Sequence<Ts..., RhsHead, RhsTail...>;
        };

        template<template<typename...> class Sequence, typename... Ts, typename Compare>
        struct merge<Sequence<Ts...>, Sequence<>, Sequence<>, Compare>
        {
            using type = Sequence<Ts...>;
        };
    }

    template<typename Lhs, typename Rhs, typename Compare = less>
    using merge = detail::merge<functor_t<Lhs>, Lhs, Rhs, Compare>;

    template<typename Lhs, typename Rhs, typename Compare = less>
    using merge_t = type_t<merge<Lhs, Rhs, Compare>>;

    namespace detail
    {
        namespace mergesort
        {
            template<typename List, typename Compare>
            struct mergesort
            {
                using left  = split_left_t< div_t<list_size<List>, int32_t<2>>::value, List>;
                using right = split_after_t<div_t<list_size<List>, int32_t<2>>::value, List>;

                using left_sorted  = type_t<mergesort<left,  Compare>>;
                using right_sorted = type_t<mergesort<right, Compare>>;

                using type = merge_t<left_sorted, right_sorted, Compare>;
            };

            template<template<typename...> class Sequence, typename Compare>
            struct mergesort<Sequence<>, Compare>
            {
                using type = Sequence<>;
            };

            template<template<typename...> class Sequence, typename T, typename Compare>
            struct mergesort<Sequence<T>, Compare>
            {
                using type = Sequence<T>;
            };

            template<template<typename...> class Sequence, typename T, typename U, typename Compare>
            struct mergesort<Sequence<T, U>, Compare>
            {
                template<typename First, typename Second, bool FirstWins = apply_t<Compare, First, Second>::value>
                struct apply
                {
                    using type = Sequence<First, Second>;
                };

                template<typename First, typename Second>
                struct apply<First, Second, false>
                {
                    using type = Sequence<Second, First>;
                };

                using type = type_t<apply<T, U>>;
            };
        }
    }

    template<typename List, typename Compare = less>
    using sort = detail::mergesort::mergesort<List, Compare>;

    template<typename List, typename Compare = less>
    using sort_t = type_t<sort<List, Compare>>;

    template<typename Key, typename Value>
    struct pair
    {
        using key = Key;
        using value = Value;
    };

    template<typename Pair>
    using key_t = typename Pair::key;
    template<typename Pair>
    using value_t = typename Pair::value;

    template<typename... Ts>
    struct inherit : Ts... {};

    template<typename... Ts>
    struct inherit<list<Ts...>> : Ts... {};

    namespace detail
    {
        template<typename Ts>
        struct aggregate;

        template<template<typename...> class Seq, typename T, typename... Ts>
        struct aggregate<Seq<T, Ts...>> : public
            aggregate<Seq<Ts...>>
        {
            aggregate()                            = delete;
            aggregate(const aggregate&)            = delete;
            aggregate(aggregate&&)                 = delete;
            aggregate& operator=(const aggregate&) = delete;
            aggregate& operator=(aggregate&&)      = delete;

            T member;
        };

        template<template<typename...> class Seq>
        struct aggregate<Seq<>>
        {
            aggregate()                            = delete;
            aggregate(const aggregate&)            = delete;
            aggregate(aggregate&&)                 = delete;
            aggregate& operator=(const aggregate&) = delete;
            aggregate& operator=(aggregate&&)      = delete;
        };
    }

    template<typename... Ts>
    using aggregate = ::ctti::meta::detail::aggregate<::ctti::meta::list<Ts...>>;

    template<typename...>
    struct map;

    template<typename... Keys, typename... Values>
    struct map<pair<Keys, Values>...>
    {
        using keys   = list<Keys...>;
        using values = list<Values...>;
        using pairs  = list<pair<Keys, Values>...>;

        struct key_not_found {};

        template<typename Key>
        using at_key = type_t<decltype(lookup(static_cast<inherit<pairs>*>(nullptr)))>;
    private:
        template<typename Key, typename Value>
        static identity<Value> lookup(pair<Key, Value>*);

        template<typename Key>
        static identity<key_not_found> lookup(...);
    };

    template<typename Map>
    using keys_t = typename Map::keys;
    template<typename Map>
    using values_t = typename Map::values;
    template<typename Map>
    using pairs_t = typename Map::pairs;
    template<typename Map, typename Key>
    using at_key = typename Map::template at_key<Key>;

    template<typename Function, typename... Ts>
    struct pack_fmap
    {
        using type = list<apply_t<Function, Ts>...>;
    };

    template<typename Function, typename List>
    struct fmap;

    template<typename Function, template<typename...> class Seq, typename... Ts>
    struct fmap<Function, Seq<Ts...>>
    {
        using type = Seq<apply_t<Function, Ts>...>;
    };

    namespace detail
    {
        template<typename Predicate, typename FilteredSeq, typename Seq>
        struct filter;

        template<typename Predicate,
                 template<typename...> class Seq, typename... Filtered, typename Head, typename... Tail>
        struct filter<Predicate, Seq<Filtered...>, Seq<Head, Tail...>>
        {
            template<typename _Head = Head, bool = value<apply_t<Predicate, _Head>>()>
            struct next
            {
                using type = Seq<Filtered..., Head>;
            };

            template<typename _Head>
            struct next<_Head, false>
            {
                using type = Seq<Filtered...>;
            };

            using type = type_t<filter<Predicate, type_t<next<>>, Seq<Tail...>>>;
        };

        template<typename Predicate,
            template<typename...> class Seq, typename... Filtered>
        struct filter<Predicate, Seq<Filtered...>, Seq<>>
        {
            using type = Seq<Filtered...>;
        };
    }

    template<typename Function, typename Seed, typename Seq>
    struct foldl;

    template<typename Function, typename Seed,
             template<typename...> class Seq, typename Head, typename... Tail>
    struct foldl<Function, Seed, Seq<Head, Tail...>>
    {
        using type = type_t<
            foldl<Function,
                  apply_t<Function, Seed, Head>,
                  Seq<Tail...>
            >
        >;
    };

    template<typename Function, typename Seed,
             template<typename...> class Seq>
    struct foldl<Function, Seed, Seq<>>
    {
        using type = Seed;
    };

    template<typename Function, typename Seed, typename Seq>
    struct foldr;

    template<typename Function, typename Seed,
        template<typename...> class Seq, typename Head, typename... Tail>
    struct foldr<Function, Seed, Seq<Head, Tail...>>
    {
        using type = apply_t<
            Function, Head,
            type_t<foldr<Function, Seed, Seq<Tail...>>>
        >;
    };

    template<typename Function, typename Seed,
        template<typename...> class Seq>
    struct foldr<Function, Seed, Seq<>>
    {
        using type = Seed;
    };

    template<typename Function, typename... Ts>
    using pack_fmap_t = type_t<pack_fmap<Function, Ts...>>;
    template<typename Function, typename Seq>
    using fmap_t = type_t<fmap<Function, Seq>>;

    template<typename Function, typename Seed, typename... Seq>
    using pack_foldl = foldl<Function, Seed, list<Seq...>>;
    template<typename Function, typename Seed, typename... Seq>
    using pack_foldl_t = type_t<pack_foldl<Function, Seed, Seq...>>;
    template<typename Function, typename Seed, typename Seq>
    using foldl_t = type_t<foldl<Function, Seed, Seq>>;

    template<typename Function, typename Seed, typename... Seq>
    using pack_foldr = foldr<Function, Seed, list<Seq...>>;
    template<typename Function, typename Seed, typename... Seq>
    using pack_foldr_t = type_t<pack_foldr<Function, Seed, Seq...>>;
    template<typename Function, typename Seed, typename Seq>
    using foldr_t = type_t<foldr<Function, Seed, Seq>>;

    template<typename Predicate, typename Seq>
    using filter = detail::filter<Predicate, apply_functor<Seq>, Seq>;
    template<typename Predicate, typename Seq>
    using filter_t = type_t<filter<Predicate, Seq>>;
    template<typename Predicate, typename... Seq>
    using pack_filter = detail::filter<Predicate, list<>, list<Seq...>>;
    template<typename Predicate, typename... Seq>
    using pack_filter_t = type_t<pack_filter<Predicate, Seq...>>;

    template<typename Bs>
    using any_of = foldl<or_, false_, Bs>;
    template<typename Bs>
    using any_of_t = foldl_t<or_, false_, Bs>;
    template<typename... Bs>
    using pack_any_of = pack_foldl<or_, false_, Bs...>;
    template<typename... Bs>
    using pack_any_of_t = pack_foldl_t<or_, false_, Bs...>;

    template<typename Bs>
    using any_of = foldl<or_, false_, Bs>;
    template<typename Bs>
    using any_of_t = foldl_t<or_, false_, Bs>;
    template<typename... Bs>
    using pack_any_of = pack_foldl<or_, false_, Bs...>;
    template<typename... Bs>
    using pack_any_of_t = pack_foldl_t<or_, false_, Bs...>;

    template<typename... Seqs>
    using join = foldl<defer<cat>, apply_functor<pack_get_t<0, Seqs...>>, apply_functor<pack_get_t<0, Seqs...>, Seqs...>>;
    template<typename... Seqs>
    using join_t = type_t<join<Seqs...>>;

    namespace detail
    {
        template<template<typename...> class Seq, std::size_t N>
        struct make_index_sequence
        {
            static constexpr std::size_t n = (N % 2) ? ((N - 1) / 2) : (N / 2);
            static constexpr std::size_t m = N - n;

            struct adder
            {
                template<typename T>
                struct apply
                {
                    using type = apply_t<add_, size_t<n>, T>;
                };
            };

            using type = cat_t<
                type_t<make_index_sequence<Seq, n>>,
                fmap_t<adder, type_t<make_index_sequence<Seq, m>>>
            >;
        };

        template<template<typename...> class Seq>
        struct make_index_sequence<Seq, 1>
        {
            using type = Seq<size_t<0>>;
        };

        template<template<typename...> class Seq>
        struct make_index_sequence<Seq, 0>
        {
            using type = Seq<>;
        };
    }

    template<std::size_t N, template<typename...> class Seq = list>
    using make_index_sequence = type_t<detail::make_index_sequence<Seq, N>>;
    template<typename... Ts>
    using make_index_sequence_for = make_index_sequence<sizeof...(Ts)>;
    template<typename Seq>
    struct to_index_sequence;
    template<template<typename...> class Seq, typename... Ts>
    struct to_index_sequence<Seq<Ts...>>
    {
        using type = make_index_sequence<sizeof...(Ts), Seq>;
    };
    template<typename Seq>
    using to_index_sequence_t = type_t<to_index_sequence<Seq>>;

    template<typename Seq>
    using make_index_sequence_from_sequence = ctti::meta::functor_t<ctti::meta::list<>, ctti::meta::to_index_sequence_t<Seq>>;

    namespace detail
    {
        template<typename Function, typename... Ts, std::size_t... Indices>
        void foreach(ctti::meta::list<Ts...>, ctti::meta::index_sequence<Indices...>, Function function)
        {
            (void)function;
            [](...){}(std::array<int, sizeof...(Ts) + 1>{{(function(ctti::meta::identity<Ts>(), ctti::meta::size_t<Indices>()), 0)..., 0}});
        }
    }

    template<typename Sequence, typename Function>
    void foreach(Function function)
    {
        ctti::meta::detail::foreach(ctti::meta::apply_functor<ctti::meta::list<>, Sequence>(), ctti::meta::make_index_sequence_from_sequence<Sequence>(), function);
    }
}

}

#endif // CTTI_UTILITY_META_H
