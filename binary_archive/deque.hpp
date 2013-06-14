#ifndef CEREAL_BINARY_ARCHIVE_DEQUE_HPP_
#define CEREAL_BINARY_ARCHIVE_DEQUE_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <deque>

namespace cereal
{
  //! Saving for std::deque to binary
  template <class T, class A>
  void save( BinaryOutputArchive & ar, std::deque<T, A> const & deque )
  {
    ar & deque.size();

    for( const auto & i : deque )
      ar & i;
  }

  //! Loading for std::deque all other types from binary (non-const version)
  template <class T, class A>
  void save( BinaryOutputArchive & ar, std::deque<T, A> & deque )
  {
    ar & deque.size();

    for( auto & i : deque )
      ar & i;
  }

  //! Loading for std::deque all other types to binary
  template <class T, class A>
  void load( BinaryInputArchive & ar, std::deque<T, A> & deque )
  {
    size_t size;
    ar & size;

    deque.resize( size );

    for( auto & i : deque )
      ar & i;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_DEQUE_HPP_
