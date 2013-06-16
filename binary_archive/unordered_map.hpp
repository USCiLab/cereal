#ifndef CEREAL_BINARY_ARCHIVE_UNORDERED_MAP_HPP_
#define CEREAL_BINARY_ARCHIVE_UNORDERED_MAP_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <unordered_map>

namespace cereal
{
  //! Saving for std::unordered_map to binary
  template <class K, class T, class H, class KE, class A> inline
  void save( BinaryOutputArchive & ar, std::unordered_map<K, T, H, KE, A> const & unordered_map )
  {
    ar & unordered_map.size();

    for( const auto & i : unordered_map )
    {
      ar & i.first;
      ar & i.second;
    }
  }

  //! Loading for std::unordered_map to binary
  template <class K, class T, class H, class KE, class A> inline
  void load( BinaryInputArchive & ar, std::unordered_map<K, T, H, KE, A> & unordered_map )
  {
    size_t size;
    ar & size;

    unordered_map.reserve( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar & key;
      ar & value;
      unordered_map.insert( {key, value} );
    }
  }

  //! Saving for std::unordered_multimap to binary
  template <class K, class T, class H, class KE, class A> inline
  void save( BinaryOutputArchive & ar, std::unordered_multimap<K, T, H, KE, A> const & unordered_multimap )
  {
    ar & unordered_multimap.size();

    for( const auto & i : unordered_multimap )
    {
      ar & i.first;
      ar & i.second;
    }
  }

  //! Loading for std::unordered_multimap to binary
  template <class K, class T, class H, class KE, class A> inline
  void load( BinaryInputArchive & ar, std::unordered_multimap<K, T, H, KE, A> & unordered_multimap )
  {
    size_t size;
    ar & size;

    unordered_multimap.reserve( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar & key;
      ar & value;
      unordered_multimap.insert( {key, value} );
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_UNORDERED_MAP_HPP_
