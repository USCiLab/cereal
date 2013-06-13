#ifndef CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
#define CEREAL_BINARY_ARCHIVE_ARRAY_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <array>

namespace cereal
{
  //! Saving for std::array primitive types to binary
  template <class T, size_t N>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  void save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    std::cout << "Saving array (arith)" << std::endl;
    ar.save_binary( array.data(), N * sizeof(T) );
  }

  //! Loading for std::array primitive types to binary
  template <class T, size_t N>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  void load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    std::cout << "Loading array (arith)" << std::endl;
    ar.load_binary( array.data(), N * sizeof(T) );
  }

  //! Saving for std::array all other types to binary
  template <class T, size_t N>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  void save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    std::cout << "Saving array" << std::endl;
    for( const auto & i : array )
      ar & i;
  }

  //! Loading for std::array all other types to binary
  template <class T, size_t N>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  void load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    std::cout << "Loading array" << std::endl;
    for( auto & i : array )
      ar & i;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
