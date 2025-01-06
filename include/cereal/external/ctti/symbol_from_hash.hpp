#ifndef CTTI_SYMBOL_FROM_HASH_HPP
#define CTTI_SYMBOL_FROM_HASH_HPP

#include <cstddef>

namespace ctti
{

template<std::uint64_t Hash>
using symbol_from_hash = decltype(ctti_symbol_from_hash(ctti::meta::uint64_t<Hash>()));

}


#endif // CTTI_SYMBOL_FROM_HASH_HPP
