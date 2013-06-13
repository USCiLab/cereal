#ifndef CEREAL_BINARY_ARCHIVE_VECTOR_HPP_
#define CEREAL_BINARY_ARCHIVE_VECTOR_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <vector>

namespace cereal
{
  //! Serialization for std::vectors of arithmetic (but not bool) types to binary
  template <class T, class A>
  typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, void>::type
  save( BinaryOutputArchive & ar, std::vector<T, A> const & vector )
  {
    std::cout << "Saving vector (arithmetic)" << std::endl;

    const size_t dataSize = std::addressof(vector.back()) - std::addressof(vector.front());

    ar & vector.size(); // number of elements
    ar & dataSize;      // size of data (may be larger due to allocator strategy)
    ar.save_binary( array.data(), size ); // actual data
  }

  //! Serialization for std::vectors of arithmetic (but not bool) types to binary
  template <class T, class A>
  typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, void>::type
  load( BinaryInputArchive & ar, std::vector<T, A> & vector )
  {
    std::cout << "Loading vector (arithmetic)" << std::endl;

    size_t dataSize;
    size_t vectorSize;
    ar & vectorSize;
    ar & dataSize;

    vector.resize( vectorSize );

    ar.load_binary( vector.data(), dataSize );
  }

  //! Serialization for all other vector types
  template <class T, class A>
  void save( BinaryOutputArchive & ar, std::vector<T, A> const & vector )
  {
    std::cout << "Saving vector" << std::endl;

    ar & vector.size(); // number of elements
    for( auto it = vector.begin(), end = vector.end(); it != end; ++it )
      ar & (*it);
  }

  //! Serialization for std::vector<bool, A> to binary
  template <class T, class A>
  void load( BinaryInputArchive & ar, std::vector<T, A> & vector )
  {
    size_t size;
    ar & size;

    vector.resize( size );
    for( auto it = vector.begin(), end = vector.end(); it != end; ++it )
      ar & (*it);
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_VECTOR_HPP_
