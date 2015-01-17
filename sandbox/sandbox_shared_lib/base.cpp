#ifndef CEREAL_DLL_USE
#define CEREAL_DLL_MAKE
#endif
#include "base.hpp"

template void Base::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version );
template void Base::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version );
