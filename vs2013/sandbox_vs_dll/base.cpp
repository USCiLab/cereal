#include "base.hpp" 

template void Base::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version ); 
template void Base::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version ); 

template <class Archive> 
void Base::serialize(Archive & ar, std::uint32_t const version) 
{ 
} 