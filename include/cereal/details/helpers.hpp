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
#ifndef CEREAL_DETAILS_HELPERS_HPP_
#define CEREAL_DETAILS_HELPERS_HPP_

#include <type_traits>
#include <cstdint>
#include <utility>

namespace cereal
{
  // ######################################################################
  namespace detail
  {
    struct NameValuePairCore {};
  }

  //! For holding name value pairs
  /*! This pairs a name (some string) with some value such that an archive
      can potentially take advantage of the pairing. */
  template <class T>
  class NameValuePair : detail::NameValuePairCore
  {
    private:
      // If we get passed an RValue, we'll just make a local copy if it here
      // otherwise, we store a reference
      using DT = typename std::decay<T>::type;
      using Type = typename std::conditional<std::is_rvalue_reference<T>::value,
                                             DT,
                                            typename std::add_lvalue_reference<DT>::type>::type;
      // prevent nested nvps
      static_assert( !std::is_base_of<detail::NameValuePairCore, T>::value,
                     "Cannot pair a name to a NameValuePair" );

    public:
      //! Constructs a new NameValuePair
      /*! @param n The name of the pair
          @param v The value to pair.  Ideally this should be an l-value reference so that
                   the value can be both loaded and saved to.  If you pass an r-value reference,
                   the NameValuePair will store a copy of it instead of a reference.  Thus you should
                   only pass r-values in cases where this makes sense, such as the result of some
                   size() call.  In either case, any constness will be stripped away */
      NameValuePair( char const * n, T && v ) : name(n), value(const_cast<Type>(v)) {}

      char const * name;
      Type value;
  };

  // ######################################################################
  //! A wrapper around data that can be serialized in a binary fashion
  /*! This class is used to demarcate data that can safely be serialized
      as a binary chunk of data.  Individual archives can then choose how
      best represent this during serialization. */
  template <class T>
  struct BinaryData
  {
    //! Internally store the pointer as a void *, keeping const if created with
    //! a const pointer
    using PT = typename std::conditional<std::is_const<typename std::remove_pointer<T>::type>::value,
                                         const void *,
                                         void *>::type;

    BinaryData( T && d, uint64_t s ) : data(d), size(s) {}

    PT data;   //!< pointer to beginning of data
    uint64_t size; //!< size in bytes
  };

  // ######################################################################
  namespace detail
  {
    // base classes for type checking
    struct OutputArchiveBase {};
    struct InputArchiveBase {};

    // forward decls for polymorphic support
    template <class Archive, class T> struct polymorphic_serialization_support;
    struct adl_tag;

    // used during saving pointers
    static const int32_t msb_32bit = 0x80000000;
    static const int32_t msb2_32bit = 0x40000000;
  }

  // ######################################################################
  //! A wrapper around size metadata
  /*! This class provides a way for archives to have more flexibility over how
      they choose to serialize size metadata for containers.  For some archive
      types, the size may be implicitly encoded in the output (e.g. JSON) and
      not need an explicit entry.  Specializing serialize or load/save for
      your archive and SizeTags allows you to choose what happens */
  template <class T>
  class SizeTag
  {
    private:
      // If we get passed an RValue, we'll just make a local copy if it here
      // otherwise, we store a reference
      using DT = typename std::decay<T>::type;
      using Type = typename std::conditional<std::is_rvalue_reference<T>::value,
                                             DT,
                                            typename std::add_lvalue_reference<DT>::type>::type;

    public:
      SizeTag( T && sz ) : size(const_cast<Type>(sz)) {}

      Type size;
  };
} // namespace cereal

#endif // CEREAL_DETAILS_HELPERS_HPP_
