#ifndef CEREAL_DLL_USE
#define CEREAL_DLL_MAKE
#endif
#include "derived.hpp"

template void Derived::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version );

template void Derived::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version );
