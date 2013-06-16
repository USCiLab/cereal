#ifndef CEREAL_BINARY_ARCHIVE_BITSET_HPP_
#define CEREAL_BINARY_ARCHIVE_BITSET_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <bitset>

namespace cereal
{
  namespace bitset_detail
  {
    enum class type : uint8_t
    {
      ulong,
      uulong,
      string
    };

  };

  //! Serializing (save) for std::bitset to binary
  template <size_t N> inline
  void save( BinaryOutputArchive & ar, std::bitset<N> const & bits )
  {
    try
    {
      auto const b = bits.to_ulong();
      ar & bitset_detail::type::ulong;
      ar & b;
    }
    catch( std::overflow_error const & e )
    {
      try
      {
        auto const b = bits.to_uulong();
        ar & bitset_detail::type::uulong;
        ar & b;
      }
      catch( std::overflow_error const & e )
      {
        ar & bitset_detail::type::string;
        ar & bits.to_string();
      }
    }
  }

  //! Serializing (load) for std::bitset to binary
  template <size_t N> inline
  void load( BinaryInputArchive & ar, std::bitset<N> & bits )
  {
    bitset_detail::type t;
    ar & t;

    switch( t )
    {
      case bitset_detail::type::ulong:
      {
        unsigned long b;
        ar & b;
        bits = std::bitset<N>( b );
        break;
      }
      case bitset_detail::type::uulong:
      {
        unsigned long long b;
        ar & b;
        bits = std::bitset<N>( b );
        break;
      }
      case bitset_detail::type::string:
      {
        std::string b;
        ar & b;
        bits = std::bitset<N>( b );
        break;
      }
      default:
        throw 1;
        // TODO: emit some kind of error
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_BITSET_HPP_
