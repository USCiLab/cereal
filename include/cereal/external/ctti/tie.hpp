#ifndef CTTI_TIE_HPP
#define CTTI_TIE_HPP

#include <ctti/symbol.hpp>
#include <ctti/detail/meta.hpp>
#include <tuple>

namespace ctti
{

namespace detail
{

template<typename Symbols, typename Refs>
struct tie_t;

template<typename... Symbols, typename... Refs>
struct tie_t<ctti::meta::list<Symbols...>, ctti::meta::list<Refs...>>
{
    constexpr tie_t(Refs&... refs) :
        _refs{refs...}
    {}

    template<typename T>
    void operator=(const T& object)
    {
        assign(object, ctti::meta::make_index_sequence_for<Symbols...>());
    }

private:
    template<std::size_t Index>
    using Symbol = ctti::meta::pack_get_t<Index, Symbols...>;


    template<std::size_t Index, typename T, typename = typename std::enable_if<
        Symbol<Index>::template is_member_of<T>()
    >::type>
    void assign_member(const T& object)
    {
        std::get<Index>(_refs) = ctti::get_member_value<Symbol<Index>>(object);
    }

    template<std::size_t Index, typename T, typename std::enable_if<
        !Symbol<Index>::template is_member_of<T>()
    >::type>
    void assign_member(const T& object)
    {}

    template<typename T, std::size_t... Index>
    void assign(const T& object, ctti::meta::index_sequence<Index...>)
    {
        [](...){}((tie_t::assign_member<Index>(object), 42)...);
    }

    std::tuple<Refs&...> _refs;
};

}

template<typename... Symbols, typename... Refs>
constexpr ctti::detail::tie_t<
    ctti::meta::list<Symbols...>, ctti::meta::list<Refs...>
> tie(Refs&... refs)
{
    return {refs...};
}

}

#endif // CTTI_TIE_HPP
