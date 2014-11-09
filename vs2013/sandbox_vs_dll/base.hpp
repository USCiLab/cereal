#pragma once 

#include <memory> 
#include <cereal/cereal.hpp> 
#include <cereal/archives/xml.hpp> 
#include <cereal/types/polymorphic.hpp> 

#if defined (_WINDLL) 
#define DECLSPECIFIER __declspec(dllexport) 
#else 
#define DECLSPECIFIER __declspec(dllimport) 
#endif 

class Base
{
  public:
    friend class cereal::access;

    template < class Archive >
    DECLSPECIFIER void serialize( Archive & ar, std::uint32_t const version );
    virtual ~Base() {}
};

extern template DECLSPECIFIER void Base::serialize<cereal::XMLInputArchive>
( cereal::XMLInputArchive & ar, std::uint32_t const version );

extern template DECLSPECIFIER void Base::serialize<cereal::XMLOutputArchive>
( cereal::XMLOutputArchive & ar, std::uint32_t const version );