/*! \file common.hpp
    \brief Support common types - always included automatically
    \ingroup OtherTypes */
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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_COMMON_HPP_
#define CEREAL_TYPES_COMMON_HPP_

#include <cereal/cereal.hpp>

namespace cereal
{
  //! Serialization for enum types
  template<class Archive, class T> inline
  typename std::enable_if<std::is_enum<T>::value, void>::type
  serialize(Archive & ar, T & t)
  {
    ar( reinterpret_cast<typename std::underlying_type<T>::type &>(t) );
  }

  //! Serialization for raw pointers
  /*! This exists only to throw a static_assert to let users know we don't support raw pointers. */
  template <class Archive, class T> inline
  void serialize( Archive &, T * & )
  {
    static_assert(!sizeof(T), "Cereal does not support serializing raw pointers - please use a smart pointer");
  }

  namespace common_detail
  {
    //! Serialization for arrays if BinaryData is supported
    /*! @internal */
    template <class Archive, class T> inline
    void serializeArray( Archive & ar, T & array, std::true_type /* binary_supported */ )
    {
      ar( binary_data( array, traits::sizeof_array<T>() * sizeof(typename std::remove_all_extents<T>::type) ) );
    }

    //! Serialization for arrays if BinaryData is not supported
    /*! @internal */
    template <class Archive, class T> inline
    void serializeArray( Archive & ar, T & array, std::false_type /* binary_supported */ )
    {
      for( auto & i : array )
        ar( i );
    }
  }

  //! Serialization for C style arrays
  template <class Archive, class T> inline
  typename std::enable_if<std::is_array<T>::value, void>::type
  serialize(Archive & ar, T & array)
  {
    common_detail::serializeArray( ar, array,
        std::integral_constant<bool, traits::is_output_serializable<BinaryData<T>, Archive>()>() );
  }
} // namespace cereal

#endif // CEREAL_TYPES_COMMON_HPP_
