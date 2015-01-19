#pragma once

#include <memory>
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>

#if defined (_WINDLL)
#define DECLSPECIFIER __declspec(dllexport)
#elif defined(MSC_VER)
#define DECLSPECIFIER __declspec(dllimport)
#else
#define DECLSPECIFIER
#endif

int doit();

class VersionTest
{
  public:
    int x;
    template <class Archive>
    void serialize( Archive & ar, const std::uint32_t /* version */ )
    { ar( x ); }
};

class Base
{
  public:
    friend class cereal::access;

    template < class Archive >
    void serialize(Archive &, std::uint32_t const) {}
    virtual ~Base() {}
};

extern template DECLSPECIFIER void Base::serialize<cereal::XMLInputArchive>
( cereal::XMLInputArchive & ar, std::uint32_t const version );

extern template DECLSPECIFIER void Base::serialize<cereal::XMLOutputArchive>
( cereal::XMLOutputArchive & ar, std::uint32_t const version );

CEREAL_CLASS_VERSION(VersionTest, 1)
