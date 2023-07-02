#ifndef CTTI_DETAIL_ENTITY_NAME_HPP
#define CTTI_DETAIL_ENTITY_NAME_HPP

#include "cstring.hpp"

namespace ctti
{

namespace detail
{

class entity_name
{
public:
    constexpr entity_name(const ctti::detail::cstring& str) :
        _str{str}
    {}

    constexpr ctti::detail::cstring str() const
    {
        return _str;
    }

    constexpr ctti::detail::cstring operator[](std::size_t i) const
    {
        return colon_scan(_str.begin(), _str.end(), i);
    }

private:
    ctti::detail::cstring _str;

    constexpr ctti::detail::cstring colon_scan(const char* begin, const char* end, std::size_t i) const
    {
        return (begin == end) ? {begin, end} :
            (i == 0) ? {begin, end}
            (colon_count == 0 && *begin == ':') ? colon_scan(++begin, end, i, ++colon_count) :
            (colon_count == 1 && *begin == ':') ? colon_scan(++begin, end, i - 1, 0)
            (
    }
};

}

}

#endif // CTTI_DETAIL_ENTITY_NAME_HPP
