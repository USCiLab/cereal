#ifndef CTTI_NAME_HPP
#define CTTI_NAME_HPP

#include <ctti/detail/cstring.hpp>
#include <ctti/detail/name_filters.hpp>

namespace ctti
{

struct name_t
{
    constexpr name_t(const ctti::detail::cstring& full_name) :
        _full_name{full_name}
    {}

    constexpr ctti::detail::cstring name() const
    {
        return ctti::detail::find_last(_full_name, "::") == _full_name.end() ?
            _full_name : ctti::detail::cstring{ctti::detail::find_last(_full_name, "::") + 2, _full_name.end()};
    }

    constexpr ctti::detail::cstring full_name() const
    {
        return _full_name;
    }

    constexpr ctti::detail::cstring full_homogeneous_name() const
    {
        return ctti::detail::filter_prefix(full_name(), "&");
    }

    constexpr ctti::detail::cstring qualifier(std::size_t i) const
    {
        return (i > 0) ? get_qualifier(
            ctti::detail::find_ith(_full_name, "::", i - 1) + 2,
            ctti::detail::find_ith(_full_name, "::", i)
        ) : (ctti::detail::find_ith(_full_name, "::", i) != _full_name.end()) ?
            ctti::detail::cstring{_full_name.begin(), ctti::detail::find_ith(_full_name, "::", i)} :
            ctti::detail::cstring{""};
    }

private:
    ctti::detail::cstring _full_name;

    constexpr ctti::detail::cstring right(const ctti::detail::cstring& str) const
    {
        return {ctti::detail::min(_full_name.end(), str.begin()), _full_name.end()};
    }

    constexpr ctti::detail::cstring left(const char* end) const
    {
        return (end < _full_name.end()) ?
            ctti::detail::cstring{_full_name.begin(), end}
        :
            ctti::detail::cstring{""};
    }

    constexpr ctti::detail::cstring get_name(const ctti::detail::cstring& name) const
    {
        return (name != "") ? name : _full_name;
    }

    constexpr ctti::detail::cstring get_qualifier(const char* begin, const char* end) const
    {
        return get_qualifier_impl(begin, end);
    }

    constexpr ctti::detail::cstring get_qualifier_impl(const char* begin, const char* end) const
    {
        return (end == _full_name.end()) ? ctti::detail::cstring{""} : ctti::detail::cstring{begin, end};
    }
};

constexpr bool operator==(const name_t& lhs, const name_t rhs)
{
    return lhs.full_name() == rhs.full_name();
}

constexpr bool operator!=(const name_t& lhs, const name_t rhs)
{
    return !(lhs == rhs);
}

}

#endif // CTTI_NAME_HPP
