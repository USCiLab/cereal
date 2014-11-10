#include "base.hpp" 

CEREAL_CLASS_VERSION(VersionTest, 1)
CEREAL_REGISTER_SHARED_LIBRARY(Sandbox)
template void Base::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version ); 
template void Base::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version ); 

template <class Archive> 
void Base::serialize(Archive & ar, std::uint32_t const version) 
{ 
} 