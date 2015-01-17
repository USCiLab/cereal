#ifndef CEREAL_DLL_USE
#define CEREAL_DLL_MAKE
#endif
#include "derived.hpp" 

//CEREAL_REGISTER_TYPE_DLL(Derived)
//CEREAL_REGISTER_SHARED_LIBRARY(Sandbox)

template void Derived::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version ); 

template void Derived::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version ); 

//template <class Archive> 
//void Derived::serialize(Archive & ar, std::uint32_t const version) 
//{ 
//  std::cout << "Hello from " << __LINE__ << std::endl;
//  ar(cereal::base_class<Base>(this)); 
//} 