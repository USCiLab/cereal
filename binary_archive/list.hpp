#ifndef CEREAL_BINARY_ARCHIVE_LIST_HPP_
#define CEREAL_BINARY_ARCHIVE_LIST_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <list>

namespace cereal
{
  //! Saving for std::list all other types to binary
  template <class T, size_t A>
  void save( BinaryOutputArchive & ar, std::list<T, A> const & list )
  {
    ar & list.size();

    for( const auto & i : list )
      ar & i;
  }

  //! Loading for std::list all other types to binary
  template <class T, size_t A>
  void load( BinaryInputArchive & ar, std::list<T, A> & list )
  {
    size_t size;
    ar & size;

    list.resize( size );

    for( auto & i : list )
      ar & i;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_LIST_HPP_
