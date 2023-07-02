#ifndef CTTI_DETAIL_HASH_HPP
#define CTTI_DETAIL_HASH_HPP

#include <cstdint>

namespace ctti
{
    namespace detail
    {
        // From https://github.com/foonathan/string_id. As usually, thanks Jonathan.

        using hash_t = std::uint64_t;

        // See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
        constexpr hash_t fnv_basis = 14695981039346656037ull;
        constexpr hash_t fnv_prime = 1099511628211ull;

        // FNV-1a 64 bit hash
        constexpr hash_t fnv1a_hash(std::size_t n, const char *str, hash_t hash = fnv_basis)
        {
            return n > 0 ? fnv1a_hash(n - 1, str + 1, (hash ^ *str) * fnv_prime) : hash;
        }

        template<std::size_t N>
        constexpr hash_t fnv1a_hash(const char (&array)[N])
        {
            return fnv1a_hash(N - 1, &array[0]);
        }
    }
}

#endif /* CTTI_DETAIL_HASH_HPP */
