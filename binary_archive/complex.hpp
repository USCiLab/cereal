#ifndef CEREAL_BINARY_ARCHIVE_COMPLEX_HPP_
#define CEREAL_BINARY_ARCHIVE_COMPLEX_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <complex>

namespace cereal
{
  //! Serializing (save) for std::complex to binary
  template <class T> inline
  void save( BinaryOutputArchive & ar, std::complex<T> const & comp )
  {
    ar & comp.real();
    ar & comp.imag();
  }

  //! Serializing (load) for std::complex to binary
  template <class T> inline
  void load( BinaryInputArchive & ar, std::complex<T> & bits )
  {
    T real, imag;
    ar & real;
    ar & imag;
    bits = {real, imag};
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_COMPLEX_HPP_
