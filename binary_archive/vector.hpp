#ifndef CEREAL_BINARY_ARCHIVE_VECTOR_HPP_
#define CEREAL_BINARY_ARCHIVE_VECTOR_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <vector>

namespace cereal
{
  //! Serialization for std::vector types to binary
  template <class T, class A>
  void save( BinaryOutputArchive & ar, std::vector<T, A> const & vector )
  {
    std::cout << "Saving vector" << std::endl;

    const size_t dataSize = std::addressof(vector.back()) - std::addressof(vector.front());

    ar & vector.size(); // number of elements
    ar & dataSize;      // size of data (may be larger due to allocator strategy)
    ar.save_binary( array.data(), size ); // actual data
  }

  //! Serialization for std::vector to binary
  template <class T>
  void load( BinaryInputArchive & ar, std::vector<T, A> & vector )
  {
    std::cout << "Loading array" << std::endl;

    size_t dataSize;
    size_t vectorSize;
    ar & vectorSize;
    ar & dataSize;

    vector.resize( vectorSize );

    ar.load_binary( vector.data(), dataSize );
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_VECTOR_HPP_
