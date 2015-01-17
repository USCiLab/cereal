#pragma once 
#include "base.hpp" 
class Derived : public Base 
{ 
  private: 
    friend class cereal::access; 
    template <class Archive> 
    void serialize(Archive & ar, std::uint32_t const version)
    {
      std::cout << "\nHello from " << __LINE__ << std::endl;
      ::cereal::detail::StaticObject<cereal::detail::bind_to_archives<Derived>>::getInstance();
  ar(cereal::base_class<Base>(this)); 
    }
}; 
extern template DECLSPECIFIER void Derived::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, std::uint32_t const version ); 
extern template DECLSPECIFIER void Derived::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, std::uint32_t const version ); 

CEREAL_REGISTER_TYPE(Derived) 