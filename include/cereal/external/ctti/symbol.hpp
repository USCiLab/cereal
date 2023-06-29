#ifndef CTTI_SYMBOL_HPP
#define CTTI_SYMBOL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detailed_nameof.hpp>
#include <ctti/detail/cstring.hpp>
#include <ctti/detail/preprocessor.hpp>
#include <memory>

#define CTTI_DEFINE_SYMBOL(name)                                             \
    struct name {                                                            \
        constexpr name() {}                                                  \
                                                                             \
        static constexpr ctti::detail::cstring symbol() {                    \
            return CTTI_PP_STR(name);                                        \
        }                                                                    \
                                                                             \
        static constexpr std::uint64_t hash() {                              \
            return symbol().hash();                                          \
        }                                                                    \
                                                                             \
        template<typename T, bool IsEnum = std::is_enum<T>::value, typename = void> \
        struct impl                                                          \
        {                                                                    \
            static constexpr bool is_member_of() {                           \
                return false;                                                \
            }                                                                \
                                                                             \
            using member_type = std::nullptr_t;                              \
                                                                             \
            static constexpr member_type get_member() {                      \
                return nullptr;                                              \
            }                                                                \
                                                                             \
            static constexpr ctti::detail::cstring member_name() {           \
                return CTTI_PP_STR(name);                                    \
            }                                                                \
        };                                                                   \
                                                                             \
        template<typename T>                                                 \
        struct impl<T, false, ::ctti::meta::void_t<decltype(&T::name)>> {    \
            static constexpr bool is_member_of() {                           \
                return true;                                                 \
            }                                                                \
                                                                             \
            using member_type = decltype(&T::name);                          \
                                                                             \
            static constexpr member_type get_member() {                      \
                return &T::name;                                             \
            }                                                                \
                                                                             \
            static constexpr ctti::detail::cstring member_name() {           \
                return ctti::nameof<CTTI_STATIC_VALUE(&T::name)>();          \
            }                                                                \
        };                                                                   \
                                                                             \
        template<typename T>                                                 \
        struct impl<T, true, ::ctti::meta::void_t<decltype(T::name)>> {      \
            static constexpr bool is_member_of() {                           \
                return true;                                                 \
            }                                                                \
                                                                             \
            using member_type = decltype(T::name);                           \
                                                                             \
            static constexpr member_type get_member() {                      \
                return T::name;                                              \
            }                                                                \
                                                                             \
            static constexpr ctti::detail::cstring member_name() {           \
                return CTTI_PP_STR(name);                                    \
            }                                                                \
        };                                                                   \
                                                                             \
        template<typename T>                                                 \
        using member_type = typename impl<T>::member_type;                   \
                                                                             \
        template<typename T>                                                 \
        static constexpr bool is_member_of() {                               \
            return impl<T>::is_member_of();                                  \
        }                                                                    \
                                                                             \
        template<typename T>                                                 \
        static constexpr member_type<T> get_member() {                       \
            return impl<T>::get_member();                                    \
        }                                                                    \
                                                                             \
        template<typename T>                                                 \
        static constexpr ctti::name_t detailed_name() {                      \
            return ctti::detailed_nameof<CTTI_STATIC_VALUE(get_member<T>())>(); \
        }                                                                    \
                                                                             \
        template<typename T>                                                 \
        static constexpr ctti::detail::cstring member_name() {               \
            return impl<T>::member_name();                                   \
        }                                                                    \
                                                                             \
        template<typename T>                                                 \
        using value_type = typename ::ctti::detail::member_traits<member_type<T>>::value_type; \
    };                                                                       \
                                                                             \
    name ctti_symbol_from_hash(::ctti::meta::uint64_t<ctti::detail::fnv1a_hash(CTTI_PP_STR(name))>)


namespace ctti
{

namespace detail
{

template<typename Member>
struct member_traits;

template<typename T, typename Class>
struct member_traits<T Class::*>
{
    using value_type = T;
    using pointer_type = T Class::*;

    template<typename Obj>
    static constexpr const value_type& get(const Obj& object, pointer_type member)
    {
        return object.*member;
    }

    template<typename Obj>
    static constexpr value_type& get(Obj& object, pointer_type member)
    {
        return object.*member;
    }

    template<typename Obj, typename Value>
    static void set(Obj& object, pointer_type member, Value&& value)
    {
        get(object, member) = std::forward<Value>(value);
    }
};

template<typename T, typename Class, typename... Args>
struct member_traits<T (Class::*)(Args...)>
{
    using value_type = T;
    using pointer_type = T (Class::*)(Args...);

    template<typename Obj, typename... _Args>
    static constexpr value_type get(const Obj& object, pointer_type member, _Args&&... args)
    {
        return (object.*member)(std::forward<_Args>(args)...);
    }

    template<typename Obj, typename... _Args>
    static void set(Obj& object, pointer_type member, _Args&&... args)
    {
        get(object, member, std::forward<_Args>(args)...);
    }
};

template<typename Class, typename Arg>
struct member_traits<void (Class::*)(Arg)>
{
    using pointer_type = void (Class::*)(Arg);
    using value_type = ctti::meta::decay_t<Arg>;

    template<typename Obj, typename _Arg>
    static void set(Obj& object, pointer_type member, _Arg&& arg)
    {
        (object.*member)(std::forward<_Arg>(arg));
    }
};

template<typename T, typename Class, typename... Args>
struct member_traits<T (Class::*)(Args...) const>
{
    using value_type = T;
    using pointer_type = T (Class::*)(Args...) const;

    template<typename Obj, typename... _Args>
    static constexpr value_type get(const Obj& object, pointer_type member, _Args&&... args)
    {
        return (object.*member)(std::forward<_Args>(args)...);
    }
};

template<typename T, typename Member, typename... Args>
constexpr auto member_traits_get(const T& object, Member member, Args&&... args)
    -> const typename member_traits<Member>::value_type&
{
    return member_traits<Member>::get(object, member, std::forward<Args>(args)...);
}

template<typename T, typename Member, typename... Args>
constexpr auto member_traits_get(T& object, Member member, Args&&... args)
    -> typename member_traits<Member>::value_type&
{
    return member_traits<Member>::get(object, member, std::forward<Args>(args)...);
}

template<typename T, typename Member, typename... Args>
void member_traits_set(T& object, Member member, Args&&... args)
{
    member_traits<Member>::set(object, member, std::forward<Args>(args)...);
}

template<std::size_t Hash>
struct no_symbol_for_hash {};

}

template<typename Symbol, typename T, typename... Args>
constexpr const typename Symbol::template value_type<T>& get_member_value(const T& object, Args&&... args)
{
    static_assert(Symbol::template is_member_of<T>(), "not a member of the class");
    return ctti::detail::member_traits_get(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

template<typename Symbol, typename T, typename... Args>
typename Symbol::template value_type<T>&
get_member_value(T& object, Args&&... args)
{
    static_assert(Symbol::template is_member_of<T>(), "not a member of the class");
    return ctti::detail::member_traits_get(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

template<typename Symbol, typename T, typename... Args>
void set_member_value(const T& object, Args&&... args)
{
    static_assert(Symbol::template is_member_of<T>(), "not a member of the class");
    ctti::detail::member_traits_set(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

template<typename Symbol, typename T, typename... Args>
void set_member_value(T& object, Args&&... args)
{
    static_assert(Symbol::template is_member_of<T>(), "not a member of the class");
    ctti::detail::member_traits_set(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

}

template<typename Hash>
ctti::detail::no_symbol_for_hash<Hash::value> ctti_symbol_from_hash(Hash);

#endif // CTTI_SYMBOL_HPP
