#ifndef CEREAL_BINARY_ARCHIVE_VECTOR_HPP_
#define CEREAL_BINARY_ARCHIVE_VECTOR_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <vector>

namespace cereal
{
  //! Serialization for std::vectors of arithmetic (but not bool) types to binary, default allocator
  template <class T, class A> inline
  typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value
                          && std::is_same<A, std::allocator<T>>::value, void>::type
  save( BinaryOutputArchive & ar, std::vector<T, A> const & vector )
  {
    ar & vector.size(); // number of elements
    ar.save_binary( vector.data(), vector.size() * sizeof(T) ); // actual data
  }

  //! Serialization for std::vectors of arithmetic (but not bool) types to binary, default allocator
  template <class T, class A> inline
  typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value
                          && std::is_same<A, std::allocator<T>>::value, void>::type
  load( BinaryInputArchive & ar, std::vector<T, A> & vector )
  {
    size_t vectorSize;
    ar & vectorSize;

    vector.resize( vectorSize );
    ar.load_binary( vector.data(), vectorSize * sizeof(T));
  }

  //! Serialization for std::vectors of arithmetic (but not bool) types to binary, special allocator
  template <class T, class A> inline
  typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value
                          && !std::is_same<A, std::allocator<T>>::value, void>::type
  save( BinaryOutputArchive & ar, std::vector<T, A> const & vector )
  {
    size_t const dataSize = std::addressof(vector.back()) - std::addressof(vector.front()) + 1;

    ar & vector.size(); // number of elements
    ar & dataSize;      // size of data (may be larger due to allocator strategy)
    ar.save_binary( vector.data(), dataSize * sizeof(T) ); // actual data
  }

  //! Serialization for std::vectors of arithmetic (but not bool) types to binary, special allocator
  template <class T, class A> inline
  typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value
                          && !std::is_same<A, std::allocator<T>>::value, void>::type
  load( BinaryInputArchive & ar, std::vector<T, A> & vector )
  {
    size_t vectorSize;
    size_t dataSize;
    ar & vectorSize;
    ar & dataSize;

    vector.resize( vectorSize );

    ar.load_binary( vector.data(), dataSize * sizeof(T));
  }

  //! Serialization for non-arithmetic (and bool) vector types to binary
  template <class T, class A> inline
  typename std::enable_if<!std::is_arithmetic<T>::value || std::is_same<T, bool>::value, void>::type
  save( BinaryOutputArchive & ar, std::vector<T, A> const & vector )
  {
    ar & vector.size(); // number of elements
    for( auto it = vector.begin(), end = vector.end(); it != end; ++it )
      ar & (*it);
  }

  //! Serialization for non-arithmetic (and bool) vector types from binary
  template <class T, class A> inline
  typename std::enable_if<!std::is_arithmetic<T>::value || std::is_same<T, bool>::value, void>::type
  load( BinaryInputArchive & ar, std::vector<T, A> & vector )
  {
    size_t size;
    ar & size;

    vector.resize( size );
    for( auto it = vector.begin(), end = vector.end(); it != end; ++it )
      ar & (*it);
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_VECTOR_HPP_
