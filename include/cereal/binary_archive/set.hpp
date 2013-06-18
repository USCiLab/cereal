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
#ifndef CEREAL_BINARY_ARCHIVE_SET_HPP_
#define CEREAL_BINARY_ARCHIVE_SET_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <set>

namespace cereal
{
  //! Saving for std::set to binary
  template <class K, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::set<K, C, A> const & set )
  {
    ar( set.size() );

    for( const auto & i : set )
      ar( i );
  }

  //! Loading for std::set to binary
  template <class K, class C, class A> inline
  void load( BinaryInputArchive & ar, std::set<K, C, A> & set )
  {
    size_t size;
    ar( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;

      ar( key );
      set.insert( key );
    }
  }

  //! Saving for std::multiset to binary
  template <class K, class C, class A> inline
  void save( BinaryOutputArchive & ar, std::multiset<K, C, A> const & multiset )
  {
    ar( multiset.size() );

    for( const auto & i : multiset )
      ar( i );
  }

  //! Loading for std::multiset to binary
  template <class K, class C, class A> inline
  void load( BinaryInputArchive & ar, std::multiset<K, C, A> & multiset )
  {
    size_t size;
    ar( size );

    for( size_t i = 0; i < size; ++i )
    {
      K key;

      ar( key );
      multiset.insert( key );
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_SET_HPP_
