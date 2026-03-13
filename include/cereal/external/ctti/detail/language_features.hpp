#ifndef CTTI_LANGUAGE_FEATURES_HPP
#define CTTI_LANGUAGE_FEATURES_HPP

#ifdef __cpp_variable_templates
#define CTTI_HAS_VARIABLE_TEMPLATES
#endif // __cpp_variable_templates

#define CTTI_HAS_CONSTEXPR_PRETTY_FUNCTION

#if defined(__GCC__) && __GCC__ < 5
#undef CTTI_HAS_CONSTEXPR_PRETTY_FUNCTION
#endif // GCC 4.x

#ifdef __clang__
#define CTTI_HAS_ENUM_AWARE_PRETTY_FUNCTION
#endif // __clang__

#endif // CTTI_LANGUAGE_FEATURES_HPP
