//
// Created by manu343726 on 4/08/15.
//

#ifndef CTTI_PRETTY_FUNCTION_HPP
#define CTTI_PRETTY_FUNCTION_HPP

#include "cstring.hpp"

#if defined(__clang__)
    #define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
    #define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define CTTI_PRETTY_FUNCTION __FUNCSIG__
#else
    #error "No support for this compiler."
#endif

namespace ctti
{

namespace pretty_function
{

template<typename T>
constexpr ctti::detail::cstring type()
{
    return {CTTI_PRETTY_FUNCTION};
}

template<typename T, T Value>
constexpr ctti::detail::cstring value()
{
    return {CTTI_PRETTY_FUNCTION};
}

}

}

#if defined(__clang__)
    #define CTTI_TYPE_PRETTY_FUNCTION_PREFIX "ctti::detail::cstring ctti::pretty_function::type() [T = "
    #define CTTI_TYPE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(__GNUC__) && !defined(__clang__)
    #define CTTI_TYPE_PRETTY_FUNCTION_PREFIX "constexpr ctti::detail::cstring ctti::pretty_function::type() [with T = "
    #define CTTI_TYPE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(_MSC_VER)
    #define CTTI_TYPE_PRETTY_FUNCTION_PREFIX "struct ctti::detail::cstring __cdecl ctti::pretty_function::type<"
    #define CTTI_TYPE_PRETTY_FUNCTION_SUFFIX ">(void)"
#else
    #error "No support for this compiler."
#endif

#define CTTI_TYPE_PRETTY_FUNCTION_LEFT (sizeof(CTTI_TYPE_PRETTY_FUNCTION_PREFIX) - 1)
#define CTTI_TYPE_PRETTY_FUNCTION_RIGHT (sizeof(CTTI_TYPE_PRETTY_FUNCTION_SUFFIX) - 1)

#if defined(__clang__)
    #define CTTI_VALUE_PRETTY_FUNCTION_PREFIX "ctti::detail::cstring ctti::pretty_function::value() [T = "
    #define CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR "; Value = "
    #define CTTI_VALUE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(__GNUC__) && !defined(__clang__)
    #define CTTI_VALUE_PRETTY_FUNCTION_PREFIX "constexpr ctti::detail::cstring ctti::pretty_function::value() [with T = "
    #define CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR "; T Value = "
    #define CTTI_VALUE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(_MSC_VER)
    #define CTTI_VALUE_PRETTY_FUNCTION_PREFIX "struct ctti::detail::cstring __cdecl ctti::pretty_function::value<"
    #define CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR "; T Value = "
    #define CTTI_VALUE_PRETTY_FUNCTION_SUFFIX ">(void)"
#else
    #error "No support for this compiler."
#endif

#define CTTI_VALUE_PRETTY_FUNCTION_LEFT (sizeof(CTTI_VALUE_PRETTY_FUNCTION_PREFIX) - 1)
#define CTTI_VALUE_PRETTY_FUNCTION_SEPARATION (sizeof(CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR) - 1)
#define CTTI_VALUE_PRETTY_FUNCTION_RIGHT (sizeof(CTTI_VALUE_PRETTY_FUNCTION_SUFFIX) - 1)

#endif //CTTI_PRETTY_FUNCTION_HPP
