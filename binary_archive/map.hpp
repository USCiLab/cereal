#ifndef CEREAL_BINARY_ARCHIVE_MAP_HPP_
#define CEREAL_BINARY_ARCHIVE_MAP_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <map>

namespace cereal
{
  //! Saving for std::map to binary
  template <class K, class T, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::map<K, T, C, A> const & map )
  {
    ar & map.size();

    for( const auto & i : map )
    {
      ar & i.first;
      ar & i.second;
    }
  }

  //! Loading for std::map to binary
  template <class K, class T, class C, class A> inline
  void load( BinaryInputArchive & ar, std::map<K, T, C, A> & map )
  {
    size_t size;
    ar & size;

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar & key;
      ar & value;
      map.insert( {key, value} );
    }
  }

  //! Saving for std::multimap to binary
  template <class K, class T, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::multimap<K, T, C, A> const & multimap )
  {
    ar & multimap.size();

    for( const auto & i : multimap )
    {
      ar & i.first;
      ar & i.second;
    }
  }

  //! Loading for std::multimap to binary
  template <class K, class T, class C, class A> inline
  void load( BinaryInputArchive & ar, std::multimap<K, T, C, A> & multimap )
  {
    size_t size;
    ar & size;

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar & key;
      ar & value;
      multimap.insert( {key, value} );
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_MAP_HPP_
