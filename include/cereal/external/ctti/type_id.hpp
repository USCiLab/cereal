//
// Created by manu343726 on 4/08/15.
//

#ifndef CTTI_HASH_HPP
#define CTTI_HASH_HPP

#include <functional>
#include "detail/hash.hpp"
#include "detail/cstring.hpp"
#include "nameof.hpp"

#ifdef CTTI_DEBUG_ID_FUNCTIONS
#include <iostream>
#include <string>

#ifndef CTTI_CONSTEXPR_ID
#define CTTI_CONSTEXPR_ID
#endif
#else
#ifndef CTTI_CONSTEXPR_ID
#define CTTI_CONSTEXPR_ID constexpr
#endif
#endif
namespace ctti
{
    struct type_id_t
    {
        constexpr type_id_t(const detail::cstring& name) :
            name_{name}
        {}

        constexpr type_id_t() :
            type_id_t{"void"}
        {}

        type_id_t& operator=(const type_id_t&) = default;

        constexpr detail::hash_t hash() const
        {
            return name_.hash();
        }

        constexpr const detail::cstring& name() const
        {
            return name_;
        }

        friend constexpr bool operator==(const type_id_t& lhs, const type_id_t& rhs)
        {
            return lhs.hash() == rhs.hash();
        }

        friend constexpr bool operator!=(const type_id_t& lhs, const type_id_t& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        detail::cstring name_;
    };

    struct unnamed_type_id_t
    {
        constexpr unnamed_type_id_t(const detail::hash_t hash) :
            _hash{hash}
        {}

        constexpr unnamed_type_id_t(const type_id_t& id) :
            _hash{id.hash()}
        {}

        unnamed_type_id_t& operator=(const unnamed_type_id_t&) = default;

        constexpr detail::hash_t hash() const
        {
	    return _hash;
        }

        friend constexpr bool operator==(const unnamed_type_id_t& lhs, const unnamed_type_id_t& rhs)
        {
            return lhs.hash() == rhs.hash();
        }

        friend constexpr bool operator!=(const unnamed_type_id_t& lhs, const unnamed_type_id_t& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        detail::hash_t _hash;
    };

    using type_index = unnamed_type_id_t; // To mimic std::type_index when using maps


    template<std::size_t N>
    constexpr ctti::unnamed_type_id_t id_from_name(const char (&typeName)[N])
    {
        return detail::fnv1a_hash(typeName);
    }

    constexpr ctti::unnamed_type_id_t id_from_name(const char* typeName, std::size_t length)
    {
        return detail::fnv1a_hash(length, typeName);
    }

    constexpr ctti::unnamed_type_id_t id_from_name(const ctti::detail::cstring& name)
    {
        return detail::fnv1a_hash(name.size(), name.begin());
    }

    // Inline to prevent ODR violation
    inline ctti::unnamed_type_id_t id_from_name(const std::string& typeName)
    {
        return detail::fnv1a_hash(typeName.size(), typeName.data());
    }

    namespace detail
    {
        template<typename T>
	CTTI_CONSTEXPR_ID ctti::type_id_t type_id()
        {
            return { ctti::nameof<T>() };
        }

        template<typename T>
        CTTI_CONSTEXPR_ID ctti::unnamed_type_id_t unnamed_type_id()
        {
            return { id_from_name(ctti::nameof<T>()) };
        }
    }

    /**
     * Returns type information at compile-time for a value
     * of type T. Decay is applied to argument type first, use
     * ctti::type_id<decltype(arg)>() to preserve references and cv qualifiers.
     */
    template<typename T>
    constexpr type_id_t type_id(T&&)
    {
        return detail::type_id<typename std::decay<T>::type>();
    }

    /**
     * Returns type information at compile-time for type T.
     */
    template<typename T>
    constexpr type_id_t type_id()
    {
        return detail::type_id<T>();
    }

    /**
     * Returns unnamed type information at compile-time for a value
     * of type T. Decay is applied to argument type first, use
     * ctti::type_id<decltype(arg)>() to preserve references and cv qualifiers.
     */
    template<typename T>
    constexpr unnamed_type_id_t unnamed_type_id(T&&)
    {
        return detail::unnamed_type_id<typename std::decay<T>::type>();
    }

    /**
     * Returns unnamed type information at compile-time for type T.
     */
    template<typename T>
    constexpr unnamed_type_id_t unnamed_type_id()
    {
        return detail::unnamed_type_id<T>();
    }

    //assert commented out, GCC 5.2.0 ICE here.
    //static_assert(type_id<void>() == type_id<void>(), "ctti::type_id_t instances must be constant expressions");
}

namespace std
{
    template<>
    struct hash<ctti::type_id_t>
    {
        constexpr std::size_t operator()(const ctti::type_id_t& id) const
        {
            // quiet warning about possible loss of data
            return std::size_t(id.hash());
        }
    };

    template<>
    struct hash<ctti::unnamed_type_id_t>
    {
        constexpr std::size_t operator()(const ctti::unnamed_type_id_t& id) const
        {
            // quiet warning about possible loss of data
            return std::size_t(id.hash());
        }
    };
}

#endif //CTTI_HASH_HPP
