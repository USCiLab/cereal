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
#ifndef CEREAL_BINARY_ARCHIVE_UNORDERED_MAP_HPP_
#define CEREAL_BINARY_ARCHIVE_UNORDERED_MAP_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <unordered_map>

namespace cereal
{
  //! Saving for std::unordered_map to binary
  template <class K, class T, class H, class KE, class A> inline
  void save( BinaryOutputArchive & ar, std::unordered_map<K, T, H, KE, A> const & unordered_map )
  {
    ar & unordered_map.size();

    for( const auto & i : unordered_map )
    {
      ar & i.first;
      ar & i.second;
    }
  }

  //! Loading for std::unordered_map to binary
  template <class K, class T, class H, class KE, class A> inline
  void load( BinaryInputArchive & ar, std::unordered_map<K, T, H, KE, A> & unordered_map )
  {
    size_t size;
    ar & size;

    unordered_map.reserve( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar & key;
      ar & value;
      unordered_map.insert( {key, value} );
    }
  }

  //! Saving for std::unordered_multimap to binary
  template <class K, class T, class H, class KE, class A> inline
  void save( BinaryOutputArchive & ar, std::unordered_multimap<K, T, H, KE, A> const & unordered_multimap )
  {
    ar & unordered_multimap.size();

    for( const auto & i : unordered_multimap )
    {
      ar & i.first;
      ar & i.second;
    }
  }

  //! Loading for std::unordered_multimap to binary
  template <class K, class T, class H, class KE, class A> inline
  void load( BinaryInputArchive & ar, std::unordered_multimap<K, T, H, KE, A> & unordered_multimap )
  {
    size_t size;
    ar & size;

    unordered_multimap.reserve( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;
      T value;

      ar & key;
      ar & value;
      unordered_multimap.insert( {key, value} );
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_UNORDERED_MAP_HPP_
