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
#ifndef CEREAL_BINARY_ARCHIVE_MAP_HPP_
#define CEREAL_BINARY_ARCHIVE_MAP_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <map>

namespace cereal
{
  //! Saving for std::map to binary
  template <class K, class T, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::map<K, T, C, A> const & map )
  {
    ar( map.size() );

    for( const auto & i : map )
    {
      ar( i.first,
          i.second );
    }
  }

  //! Loading for std::map to binary
  template <class K, class T, class C, class A> inline
  void load( BinaryInputArchive & ar, std::map<K, T, C, A> & map )
  {
    size_t size;
    ar( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar( key, value );
      map.insert( {key, value} );
    }
  }

  //! Saving for std::multimap to binary
  template <class K, class T, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::multimap<K, T, C, A> const & multimap )
  {
    ar( multimap.size() );

    for( const auto & i : multimap )
    {
      ar( i.first,
          i.second );
    }
  }

  //! Loading for std::multimap to binary
  template <class K, class T, class C, class A> inline
  void load( BinaryInputArchive & ar, std::multimap<K, T, C, A> & multimap )
  {
    size_t size;
    ar( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar( key, value );
      multimap.insert( {key, value} );
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_MAP_HPP_
