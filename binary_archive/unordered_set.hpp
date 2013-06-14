#ifndef CEREAL_BINARY_ARCHIVE_UNORDERED_SET_HPP_
#define CEREAL_BINARY_ARCHIVE_UNORDERED_SET_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <unordered_set>

namespace cereal
{
  //! Saving for std::unordered_set all other types to binary
  template <class K, class H, class KE, class A>
  void save( BinaryOutputArchive & ar, std::unordered_set<K, H, KE, A> const & unordered_set )
  {
    ar & unordered_set.size();

    for( const auto & i : unordered_set )
      ar & i;
  }

  //! Loading for std::unordered_set all other types to binary
  template <class K, class H, class KE, class A>
  void load( BinaryInputArchive & ar, std::unordered_set<K, H, KE, A> & unordered_set )
  {
    size_t size;
    ar & size;

    unordered_set.reserve( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;

      ar & key;
      unordered_set.insert( key );
    }
  }

  //! Saving for std::unordered_multiset all other types to binary
  template <class K, class H, class KE, class A>
  void save( BinaryOutputArchive & ar, std::unordered_multiset<K, H, KE, A> const & unordered_multiset )
  {
    ar & multiset.size();

    for( const auto & i : multiset )
      ar & i;
  }

  //! Loading for std::multiset all other types to binary
  template <class K, class H, class KE, class A>
  void load( BinaryInputArchive & ar, std::multiset<K, H, KE, A> & multiset )
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

#endif // CEREAL_BINARY_ARCHIVE_UNORDERED_SET_HPP_
