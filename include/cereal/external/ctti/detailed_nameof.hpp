#ifndef CTTI_DETAILED_NAMEOF_HPP
#define CTTI_DETAILED_NAMEOF_HPP

#include <ctti/nameof.hpp>
#include <ctti/name.hpp>
#include <ctti/detail/language_features.hpp>

namespace ctti
{


template<typename T>
constexpr name_t detailed_nameof()
{
    return {ctti::nameof<T>()};
}

template<typename T, T Value>
constexpr name_t detailed_nameof()
{
    return {ctti::nameof<T, Value>()};
}

#ifdef CTTI_HAS_VARIABLE_TEMPLATES
template<typename T>
constexpr ctti::name_t detailed_nameof_v = ctti::detailed_nameof<T>();

// CONSIDER USING detailed_nameof_v<CTTI_STATIC_VALUE(value)> INSTEAD
template<typename T, T Value>
constexpr ctti::name_t detailed_nameof_value_v = ctti::detailed_nameof<T, Value>();
#endif // CTTI_HAS_VARIABLE_TEMPLATES

}

#endif // CTTI_DETAILED_NAMEOF_HPP
