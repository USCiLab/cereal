#ifndef CTTI_NAMEOF_HPP
#define CTTI_NAMEOF_HPP

#include "detail/pretty_function.hpp"
#include "detail/name_filters.hpp"
#include "detail/meta.hpp"
#include "detail/language_features.hpp"
#include "type_tag.hpp"
#include "static_value.hpp"
#include <type_traits>
#include <cstdint>

namespace ctti
{

template<typename T>
constexpr ctti::detail::cstring nameof();

template<typename T, T Value>
constexpr ctti::detail::cstring nameof();

namespace detail
{

template<typename T, typename = void>
struct TypeNameLength :
    std::integral_constant<
        std::size_t,
        ctti::nameof<T>().length()
    >
{};

template<typename R, typename Class>
struct TypeNameLength<R Class::*, void> :
    std::integral_constant<
        std::size_t,
        ctti::nameof<R Class::*>().length()
    >
{};

template<typename Enum>
struct TypeNameLength<Enum, typename std::enable_if<std::is_enum<Enum>::value>::type> :
    std::integral_constant<
        std::size_t,
        ctti::nameof<Enum>().length()
    >
{};

}

template<typename T, T Value>
constexpr ctti::detail::cstring nameof();

namespace detail
{

template<typename T>
struct another_level_of_indirection {};

template<typename T, typename = void>
struct nameof_impl
{
    static constexpr ctti::detail::cstring apply()
    {
        return ctti::detail::filter_typename_prefix(ctti::pretty_function::type<T>().pad(
            CTTI_TYPE_PRETTY_FUNCTION_LEFT,
            CTTI_TYPE_PRETTY_FUNCTION_RIGHT
        ));
    }
};

template<typename T>
struct nameof_impl<T, ctti::meta::void_t<decltype(T::ctti_nameof())>>
{
    static constexpr ctti::detail::cstring apply()
    {
        return T::ctti_nameof();
    }
};

template<typename T, T Value>
struct nameof_impl<another_level_of_indirection<ctti::static_value<T, Value>>, void>
{
    static constexpr ctti::detail::cstring apply()
    {
        return ctti::pretty_function::value<T, Value>().pad(
            CTTI_VALUE_PRETTY_FUNCTION_LEFT + ctti::detail::TypeNameLength<T>::value + CTTI_VALUE_PRETTY_FUNCTION_SEPARATION,
            CTTI_VALUE_PRETTY_FUNCTION_RIGHT
        );
    }
};

template<typename T, T Value>
struct nameof_impl<ctti::static_value<T, Value>, void>
{
    static constexpr ctti::detail::cstring apply()
    {
        return ctti::nameof<T, Value>();
    }
};

}

template<typename T>
constexpr ctti::detail::cstring ctti_nameof(ctti::type_tag<T>)
{
    return ctti::detail::nameof_impl<T>::apply();
}

template<typename T, T Value>
constexpr ctti::detail::cstring ctti_nameof(ctti::static_value<T, Value>)
{
    return ctti::detail::nameof_impl<ctti::detail::another_level_of_indirection<ctti::static_value<T, Value>>>::apply();
}

template<typename T>
constexpr ctti::detail::cstring nameof()
{
    using namespace ctti;
    return ctti_nameof(ctti::type_tag<T>());
}

template<typename T, T Value>
constexpr ctti::detail::cstring nameof()
{
    using namespace ctti;
    return ctti_nameof(ctti::static_value<T, Value>());
}

#ifdef CTTI_HAS_VARIABLE_TEMPLATES
template<typename T>
constexpr ctti::detail::cstring nameof_v = ctti::nameof<T>();

// CONSIDER USING nameof_v<CTTI_STATIC_VALUE(value)> INSTEAD
template<typename T, T Value>
constexpr ctti::detail::cstring nameof_value_v = ctti::nameof<T, Value>();
#endif // CTTI_HAS_VARIABLE_TEMPLATES

}

namespace std
{
constexpr ctti::detail::cstring ctti_nameof(ctti::type_tag<std::string>)
{
    return "std::string";
}
}

#endif // CTTI_NAMEOF_HPP
