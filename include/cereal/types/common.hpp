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
  namespace common_detail
  {
    //! Serialization for arrays if BinaryData is supported and we are arithmetic
    /*! @internal */
    template <class Archive, class T> inline
    void serializeArray( Archive & ar, T & array, std::true_type /* binary_supported */ )
    {
      ar( binary_data( array, sizeof(array) ) );
    }

    //! Serialization for arrays if BinaryData is not supported or we are not arithmetic
    /*! @internal */
    template <class Archive, class T> inline
    void serializeArray( Archive & ar, T & array, std::false_type /* binary_supported */ )
    {
      for( auto & i : array )
        ar( i );
    }

    /*! @internal */
    template <class T, bool IsEnum>
    struct enum_underlying_type_impl : std::false_type {};

    /*! @internal */
    template <class T>
    struct enum_underlying_type_impl<T, true> { using type = typename std::underlying_type<T>::type; };

    //! Helper for getting the type of an enum
    /* This will work even if T is not an enum - in that case the type will be something
       irrelevent
       @internal */
    template <class T>
    struct enum_underlying_type
    {
      using type = typename enum_underlying_type_impl<T, std::is_enum<T>::value>::type;
    };
  }

  //! Saving for enum types
  template <class Archive, class T> inline
  typename std::enable_if<std::is_enum<typename traits::strip_minimal<T>::type>::value,
           typename common_detail::enum_underlying_type<typename traits::strip_minimal<T>::type>::type>::type
  save_minimal( Archive const &, T const & t )
  {
    return static_cast<typename std::underlying_type<T>::type>(t);
  }

  //! Loading for enum types
  template <class Archive, class T> inline
  typename std::enable_if<std::is_enum<typename traits::strip_minimal<T>::type>::value, void>::type
  load_minimal( Archive const &, T & t,
                typename common_detail::enum_underlying_type<typename traits::strip_minimal<T>::type>::type const & value )
  {
    t = static_cast<T>(value);
  }

  //! Serialization for raw pointers
  /*! This exists only to throw a static_assert to let users know we don't support raw pointers. */
  template <class Archive, class T> inline
  void serialize( Archive &, T * & )
  {
    static_assert(cereal::traits::detail::delay_static_assert<T>::value,
      "Cereal does not support serializing raw pointers - please use a smart pointer");
  }

  //! Serialization for C style arrays
  template <class Archive, class T> inline
  typename std::enable_if<std::is_array<T>::value, void>::type
  serialize(Archive & ar, T & array)
  {
    common_detail::serializeArray( ar, array,
        std::integral_constant<bool, traits::is_output_serializable<BinaryData<T>, Archive>::value &&
                                     std::is_arithmetic<typename std::remove_all_extents<T>::type>::value>() );
  }
} // namespace cereal

#endif // CEREAL_TYPES_COMMON_HPP_
