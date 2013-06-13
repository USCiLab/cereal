#ifndef CEREAL_BINARY_ARCHIVE_FORWARD_LIST_HPP_
#define CEREAL_BINARY_ARCHIVE_FORWARD_LIST_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <forward_list>

namespace cereal
{
  //! Saving for std::forward_list all other types to binary
  template <class T, size_t A>
  void save( BinaryOutputArchive & ar, std::forward_list<T, A> const & forward_list )
  {
    ar & forward_list.size();

    for( const auto & i : forward_list )
      ar & i;
  }

  //! Loading for std::forward_list all other types to binary
  template <class T, size_t A>
  void load( BinaryInputArchive & ar, std::forward_list<T, A> & forward_list )
  {
    size_t size;
    ar & size;

    forward_list.resize( size );

    for( auto & i : forward_list )
      ar & i;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_FORWARD_LIST_HPP_
