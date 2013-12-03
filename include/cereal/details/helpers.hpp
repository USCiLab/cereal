/*! \file helpers.hpp
    \brief Internal helper functionality
    \ingroup Internal */
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
#ifndef CEREAL_DETAILS_HELPERS_HPP_
#define CEREAL_DETAILS_HELPERS_HPP_

#include <type_traits>
#include <cstdint>
#include <utility>

namespace cereal
{
  //! The size type used by cereal
  /*! To ensure compatability between 32, 64, etc bit machines, we need to use
   * a fixed size type instead of size_t, which may vary from machine to
   * machine. */
  typedef uint64_t size_type;

  // forward decls
  class BinaryOutputArchive;
  class BinaryInputArchive;

  // ######################################################################
  namespace detail
  {
    struct NameValuePairCore {};
  }

  //! For holding name value pairs
  /*! This pairs a name (some string) with some value such that an archive
      can potentially take advantage of the pairing.

      In serialization functions, NameValuePairs are usually created like so:
      @code{.cpp}
      struct MyStruct
      {
        int a, b, c, d, e;

        template<class Archive>
        void serialize(Archive & archive)
        {
          archive( CEREAL_NVP(a),
                   CEREAL_NVP(b),
                   CEREAL_NVP(c),
                   CEREAL_NVP(d),
                   CEREAL_NVP(e) );
        }
      };
      @endcode

      Alternatively, you can give you data members custom names like so:
      @code{.cpp}
      struct MyStruct
      {
        int a, b, my_embarrassing_variable_name, d, e;

        template<class Archive>
        void serialize(Archive & archive)
        {
          archive( CEREAL_NVP(a),
                   CEREAL_NVP(b),
                   cereal::make_nvp("var", my_embarrassing_variable_name) );
                   CEREAL_NVP(d),
                   CEREAL_NVP(e) );
        }
      };
      @endcode

      There is a slight amount of overhead to creating NameValuePairs, so there
      is a third method which will elide the names when they are not needed by
      the Archive:

      @code{.cpp}
      struct MyStruct
      {
        int a, b;

        template<class Archive>
        void serialize(Archive & archive)
        {
          archive( cereal::make_nvp<Archive>(a),
                   cereal::make_nvp<Archive>(b) );
        }
      };
      @endcode

      This third method is generally only used when providing generic type
      support.  Users writing their own serialize functions will normally
      explicitly control whether they want to use NVPs or not.

      @internal */
  template <class T>
  class NameValuePair : detail::NameValuePairCore
  {
    private:
      // If we get passed an RValue, we'll just make a local copy if it here
      // otherwise, we store a reference.  If we were passed an array, don't
      // decay the type - keep it as an array, and then proceed as normal
      // with the RValue business
      typedef typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
                                        typename std::remove_cv<T>::type,
                                        typename std::decay<T>::type>::type DT;
      typedef typename std::conditional<std::is_rvalue_reference<T>::value,
                                        DT,
                                        typename std::add_lvalue_reference<DT>::type>::type Type;
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
                   size() call.  In either case, any constness will be stripped away
      @internal */
      NameValuePair( char const * n, T && v ) : name(n), value(const_cast<Type>(v)) {}

      char const * name;
      Type value;
  };

  //! A specialization of make_nvp<> that simply forwards the value for binary archives
  /*! @relates NameValuePair
      @internal */
  template<class Archive, class T> inline
  typename
  std::enable_if<std::is_same<Archive, ::cereal::BinaryInputArchive>::value ||
                 std::is_same<Archive, ::cereal::BinaryOutputArchive>::value,
  T && >::type
  make_nvp( const char *, T && value )
  {
    return std::forward<T>(value);
  }

  //! A specialization of make_nvp<> that actually creates an nvp for non-binary archives
  /*! @relates NameValuePair
      @internal */
  template<class Archive, class T> inline
  typename
  std::enable_if<!std::is_same<Archive, ::cereal::BinaryInputArchive>::value &&
                 !std::is_same<Archive, ::cereal::BinaryOutputArchive>::value,
  NameValuePair<T> >::type
  make_nvp( const char * name, T && value)
  {
    return {name, std::forward<T>(value)};
  }

  //! Convenience for creating a templated NVP
  /*! For use in inteneral generic typing functions which have an
      Archive type declared
      @internal */
#define _CEREAL_NVP(name, value) ::cereal::make_nvp<Archive>(name, value)

  // ######################################################################
  //! A wrapper around data that can be serialized in a binary fashion
  /*! This class is used to demarcate data that can safely be serialized
      as a binary chunk of data.  Individual archives can then choose how
      best represent this during serialization.

      @internal */
  template <class T>
  struct BinaryData
  {
    //! Internally store the pointer as a void *, keeping const if created with
    //! a const pointer
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<T>::type>::value,
                                      const void *,
                                      void *>::type PT;

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
      your archive and SizeTags allows you to choose what happens.

      @internal */
  template <class T>
  class SizeTag
  {
    private:
      // If we get passed an RValue, we'll just make a local copy if it here
      // otherwise, we store a reference
      typedef typename std::decay<T>::type DT;
      typedef typename std::conditional<std::is_rvalue_reference<T>::value,
                                        DT,
                                        typename std::add_lvalue_reference<DT>::type>::type Type;

    public:
      SizeTag( T && sz ) : size(const_cast<Type>(sz)) {}

      Type size;
  };

  // ######################################################################
  //! A wrapper around a key and value for serializing data into maps.
  /*! This class just provides a grouping of keys and values into a struct for
      human readable archives. For example, XML archives will use this wrapper
      to write maps like so:

      @code{.xml}
      <mymap>
        <item0>
          <key>MyFirstKey</key>
          <value>MyFirstValue</value>
        </item0>
        <item1>
          <key>MySecondKey</key>
          <value>MySecondValue</value>
        </item1>
      </mymap>
      @endcode

      \sa make_map_item
      @internal */
  template <class Key, class Value>
  struct MapItem
  {
    typedef typename std::decay<Key>::type DecayKey;
    typedef typename std::conditional<
      std::is_rvalue_reference<Key>::value,
      DecayKey,
      typename std::add_lvalue_reference<DecayKey>::type>::type KeyType;

    typedef typename std::decay<Value>::type DecayValue;
    typedef typename std::conditional<
      std::is_rvalue_reference<Value>::value,
      DecayValue,
      typename std::add_lvalue_reference<DecayValue>::type>::type ValueType;

    //! Construct a MapItem from a key and a value
    /*! @internal */
    MapItem( Key && key, Value && value ) : key(const_cast<KeyType>(key)), value(const_cast<ValueType>(value)) {}

    KeyType key;
    ValueType value;

    //! Serialize the MapItem with the NVPs "key" and "value"
    template <class Archive> inline
    void serialize(Archive & archive)
    {
      archive( make_nvp<Archive>("key",   key),
               make_nvp<Archive>("value", value) );
    }
  };

  //! Create a MapItem so that human readable archives will group keys and values together
  /*! @internal
      @relates MapItem */
  template <class KeyType, class ValueType> inline
  MapItem<KeyType, ValueType> make_map_item(KeyType && key, ValueType && value)
  {
    return {std::forward<KeyType>(key), std::forward<ValueType>(value)};
  }

  //! Defines a class version for some type
  /*! This is part of the Boost Transition Layer and is not the recommended way
      of using cereal.  This works identically to how it does in Boost serialization,
      providing a version number associated with some type that is available by specifying
      a second parameter to serialize, save, or load.

      If you are using the two parameter versions of serialization functions, a default
      value of 0 will be used for version unless you specify otherwise with this macro.

      The recommended way of performing versioning in cereal is to implement it yourself
      on a class by class basis as required, creating a thin wrapper if you do not have
      access to the internals of a class. */
  #define CEREAL_CLASS_VERSION(TYPE, VERSION_NUMBER)                                          \
  namespace cereal { namespace detail {                                                       \
    template <> struct Version<TYPE> { static const std::uint32_t version = VERSION_NUMBER; };\
  } } // end namespaces
} // namespace cereal

#endif // CEREAL_DETAILS_HELPERS_HPP_
