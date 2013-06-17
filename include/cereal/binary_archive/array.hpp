/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
#define CEREAL_BINARY_ARCHIVE_ARRAY_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <array>

namespace cereal
{
  //! Saving for std::array primitive types to binary
  template <class T, size_t N> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    ar.saveBinary( array.data(), N * sizeof(T) );
  }

  //! Loading for std::array primitive types to binary
  template <class T, size_t N> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    ar.loadBinary( array.data(), N * sizeof(T) );
  }

  //! Saving for std::array all other types to binary
  template <class T, size_t N> inline
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  save( BinaryOutputArchive & ar, std::array<T, N> const & array )
  {
    for( auto const & i : array )
      ar & i;
  }

  //! Loading for std::array all other types to binary
  template <class T, size_t N> inline
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  load( BinaryInputArchive & ar, std::array<T, N> & array )
  {
    for( auto & i : array )
      ar & i;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_ARRAY_HPP_
