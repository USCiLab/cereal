#include "derived.hpp" 

//CEREAL_REGISTER_TYPE(Derived) 

template void Derived::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version ); 

template void Derived::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version ); 

template <class Archive> 
void Derived::serialize(Archive & ar, std::uint32_t const version) 
{ 
  ar(cereal::base_class<Base>(this)); 
} 