/*! \file bitset.hpp
    \brief Support for types found in \<bitset\>
    \ingroup STLSupport */
/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_BITSET_HPP_
#define CEREAL_TYPES_BITSET_HPP_

#include <cereal/cereal.hpp>
#include <bitset>

namespace cereal
{
  namespace bitset_detail
  {
    //! The type the bitset is encoded with
    /*! @internal */
    enum class type : uint8_t
    {
      bits
    };
  }

  //! Serializing (save) for std::bitset
  template <class Archive, size_t N> inline
  void CEREAL_SAVE_FUNCTION_NAME( Archive & ar, std::bitset<N> const & bits )
  {
    ar( CEREAL_NVP_("type", bitset_detail::type::bits) );
    char chunk = 0;
    size_t bit;
    int bitmask = 0x80;
    for ( bit = 0; bit < N; bit++ )
    {
      if (bits[bit]) {
        chunk |= bitmask;
      }
      bitmask >>= 1;
      if (bitmask == 0) {
        // Save chunk, reset bitmask
        ar (chunk);
        chunk = 0;
        bitmask = 0x80;
      }
    }
    if (bitmask != 0x80) {
      // Get the reminder if we didn't end in whole byte,
      // bitmask 0x80 means that chunk has unsaved data
      ar(chunk);
    }
  }

  //! Serializing (load) for std::bitset
  template <class Archive, size_t N> inline
  void CEREAL_LOAD_FUNCTION_NAME( Archive & ar, std::bitset<N> & bits )
  {
    bitset_detail::type t;
    ar( CEREAL_NVP_("type", t) );

    switch( t )
    {
      case bitset_detail::type::bits:
      {
        char chunk = 0;
        size_t bit;
        int bitmask = 0;
        for ( bit = 0; bit < N; bit++ )
        {
          if (bitmask == 0x0)
          {
            ar( chunk );
            bitmask = 0x80;
          }
          if (chunk & bitmask) {
            bits[bit] = 1;
          }
          bitmask >>= 1;
        }
        break;
      }
      default:
        throw Exception("Invalid bitset data representation");
    }
  }
} // namespace cereal

#endif // CEREAL_TYPES_BITSET_HPP_
