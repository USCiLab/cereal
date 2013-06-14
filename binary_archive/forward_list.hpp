#ifndef CEREAL_BINARY_ARCHIVE_FORWARD_LIST_HPP_
#define CEREAL_BINARY_ARCHIVE_FORWARD_LIST_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <forward_list>

namespace cereal
{
  //! Saving for std::forward_list all other types to binary
  template <class T, class A>
  void save( BinaryOutputArchive & ar, std::forward_list<T, A> const & forward_list )
  {
    // save position for size of list
    ar.pushPosition(sizeof(size_t));

    // write the list
    size_t size = 0;
    for( const auto & i : forward_list )
    {
      ar & i;
      ++size;
    }

    // write the size
    ar.popPosition();
    ar & size;
    ar.resetPosition();
  }

  //! Saving for std::forward_list all other types to binary (non-const version)
  template <class T, class A>
  void save( BinaryOutputArchive & ar, std::forward_list<T, A> & forward_list )
  {
    // save position for size of list
    ar.pushPosition(sizeof(size_t));

    // write the list
    size_t size = 0;
    for( auto & i : forward_list )
    {
      ar & i;
      ++size;
    }

    // write the size
    ar.popPosition();
    ar & size;
    ar.resetPosition();
  }

  //! Loading for std::forward_list all other types from binary
  template <class T, class A>
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
