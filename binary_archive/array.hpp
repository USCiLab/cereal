#ifndef CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
#define CEREAL_BINARY_ARCHIVE_ARRAY_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <array>

namespace cereal
{
  //! Saving for std::array primitive types to binary
  template <class T, size_t N>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    ar.save_binary( array.data(), N * sizeof(T) );
  }

  //! Loading for std::array primitive types to binary
  template <class T, size_t N>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    ar.load_binary( array.data(), N * sizeof(T) );
  }

  //! Saving for std::array all other types to binary
  template <class T, size_t N>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    for( auto const & i : array )
      ar & i;
  }

  //! Loading for std::array all other types to binary
  template <class T, size_t N>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    for( auto & i : array )
      ar & i;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
