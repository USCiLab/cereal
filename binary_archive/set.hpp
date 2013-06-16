#ifndef CEREAL_BINARY_ARCHIVE_SET_HPP_
#define CEREAL_BINARY_ARCHIVE_SET_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <set>

namespace cereal
{
  //! Saving for std::set to binary
  template <class K, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::set<K, C, A> const & set )
  {
    ar & set.size();

    for( const auto & i : set )
      ar & i;
  }

  //! Loading for std::set to binary
  template <class K, class C, class A> inline
  void load( BinaryInputArchive & ar, std::set<K, C, A> & set )
  {
    size_t size;
    ar & size;

    for( size_t i = 0; i < size; ++i )
    {
      K key;

      ar & key;
      set.insert( key );
    }
  }

  //! Saving for std::multiset to binary
  template <class K, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::multiset<K, C, A> const & multiset )
  {
    ar & multiset.size();

    for( const auto & i : multiset )
      ar & i;
  }

  //! Loading for std::multiset to binary
  template <class K, class C, class A> inline
  void load( BinaryInputArchive & ar, std::multiset<K, C, A> & multiset )
  {
    size_t size;
    ar & size;

    for( size_t i = 0; i < size; ++i )
    {
      K key;

      ar & key;
      multiset.insert( key );
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_SET_HPP_
