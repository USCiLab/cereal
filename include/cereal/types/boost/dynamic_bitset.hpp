/*! \file boost_dynamic_bitset.hpp
    \brief Support for types found in \<boost/dynamic_bitset.cpp\>
    \ingroup OtherTypes */
/*
  Copyright (c) 2014, Steve Hickman
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
#ifndef CEREAL_TYPES_BOOST_DYNAMIC_BITSET_HPP_
#define CEREAL_TYPES_BOOST_DYNAMIC_BITSET_HPP_

#include <cereal/cereal.hpp>
#include <boost/dynamic_bitset.hpp>

namespace cereal
{
   namespace boost_dynamic_bitset_detail
   {
      //! The type the bitset is encoded with
      /*! @internal */
      enum class type : uint8_t
      {
         ulong,
         //      ullong,
         string
      };
   }

   //! Serializing (save) for boost::dynamic_bitset
   template <class Archive, typename Block> inline
      void save(Archive & ar, ::boost::dynamic_bitset<Block> const & bits)
   {
      try
      {
         auto const b = bits.to_ulong();
         ar(CEREAL_NVP_("type", boost_dynamic_bitset_detail::type::ulong));
         ar(CEREAL_NVP_("size", bits.size()));
         ar(CEREAL_NVP_("data", b));
      }
      catch (std::overflow_error const &)
      {
         // try
         // {
         // auto const b = bits.to_ullong();
         // ar( CEREAL_NVP_("type", boost_dynamic_bitset_detail::type::ullong) );
         // ar( CEREAL_NVP_("data", b) );
         // }
         // catch( std::overflow_error const & )
         {
            ar(CEREAL_NVP_("type", boost_dynamic_bitset_detail::type::string));
            std::string s;
            to_string(bits, s);
            ar(CEREAL_NVP_("data", s));
            //        ar( CEREAL_NVP_("data", bits.to_string()) );
         }
      }
   }

   //! Serializing (load) for boost::dynamic_bitset
   template <class Archive, typename Block> inline
      void load(Archive & ar, ::boost::dynamic_bitset<Block> & bits)
   {
      boost_dynamic_bitset_detail::type t;
      ar(CEREAL_NVP_("type", t));

      switch (t)
      {
      case boost_dynamic_bitset_detail::type::ulong:
      {
         unsigned long b;
         typename ::boost::dynamic_bitset<Block>::size_type sz;
         ar(CEREAL_NVP_("size", sz));
         ar(CEREAL_NVP_("data", b));
         bits = ::boost::dynamic_bitset<>(sz, b);
         break;
      }
         // case boost_dynamic_bitset_detail::type::ullong:
         // {
         // unsigned long long b;
         // ar( CEREAL_NVP_("data", b) );
         // bits = boost::dynamic_bitset<N>( b );
         // break;
         // }
      case boost_dynamic_bitset_detail::type::string:
      {
         std::string b;
         ar(CEREAL_NVP_("data", b));
         bits = ::boost::dynamic_bitset<>(b);
         break;
      }
      default:
         throw Exception("Invalid bitset data representation");
      }
   }
} // namespace cereal

#endif // CEREAL_TYPES_BOOST_DYNAMIC_BITSET_HPP_
