#ifndef CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
#define CEREAL_BINARY_ARCHIVE_ARRAY_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <array>

namespace cereal
{
  //! Serialization for std::array types to binary
  template <class T, size_t N>
  void save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    std::cout << "Saving array" << std::endl;
    ar.save_binary( array.data(), N * sizeof(T) );
  }

  //! Serialization for std::array to binary
  template <class T, size_t N>
  void load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    std::cout << "Loading array" << std::endl;
    ar.load_binary( array.data(), N * sizeof(T) );
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
