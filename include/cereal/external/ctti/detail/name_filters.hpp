#ifndef CTTI_DETAIL_NAMEFILTERS_HPP
#define CTTI_DETAIL_NAMEFILTERS_HPP

#include "cstring.hpp"

namespace ctti
{

namespace detail
{

constexpr ctti::detail::cstring filter_prefix(const ctti::detail::cstring& str, const ctti::detail::cstring& prefix)
{
    return str.size() >= prefix.size() ? (str(0, prefix.size()) == prefix ? str(prefix.size(), str.size()) : str) : str;
}

constexpr ctti::detail::cstring leftpad(const ctti::detail::cstring& str)
{
    return (str.size() > 0 && str[0] == ' ') ? leftpad(str(1, str.size())) : str;
}

constexpr ctti::detail::cstring filter_class(const ctti::detail::cstring& type_name)
{
    return leftpad(filter_prefix(leftpad(type_name), "class"));
}

constexpr ctti::detail::cstring filter_struct(const ctti::detail::cstring& type_name)
{
    return leftpad(filter_prefix(leftpad(type_name), "struct"));
}

constexpr ctti::detail::cstring filter_typename_prefix(const ctti::detail::cstring& type_name)
{
    return filter_struct(filter_class(type_name));
}

namespace
{

constexpr const char* find_ith_impl(const ctti::detail::cstring& name, const ctti::detail::cstring& substring, const char* res, std::size_t i, bool infinite = false)
{
    return (name.length() >= substring.length()) ?
        ((name(0, substring.length()) == substring) ?
            ((i == 0) ?
                name.begin()
            :
                find_ith_impl(name(substring.length(), name.length()), substring, name.begin(), i - 1, infinite))
        :
            find_ith_impl(name(1, name.length()), substring, res, i, infinite))
        :
            (!infinite) ? name.end() : res;
}

}

constexpr const char* find_ith(const ctti::detail::cstring& name, const ctti::detail::cstring& substring, std::size_t i)
{
    return find_ith_impl(name, substring, name.end(), i);
}

constexpr const char* find_last(const ctti::detail::cstring& name, const ctti::detail::cstring& substring)
{
    return find_ith_impl(name, substring, name.end(), -1, true);
}

constexpr const char* find(const ctti::detail::cstring& name, const ctti::detail::cstring& substring)
{
    return find_ith(name, substring, 0);
}



}

}

#endif // CTTI_DETAIL_NAMEFILTERS_HPP
