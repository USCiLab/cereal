/*! \file helpers.hpp
    \brief Internal helper functionality
    \ingroup Internal */
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
#ifndef CEREAL_DETAILS_HELPERS_HPP_
#define CEREAL_DETAILS_HELPERS_HPP_

#include <type_traits>
#include <cstdint>
#include <utility>
#include <memory>
#include <unordered_map>
#include <stdexcept>

#include <cereal/macros.hpp>
#include <cereal/details/static_object.hpp>

namespace cereal
{
  // ######################################################################
  //! An exception class thrown when things go wrong at runtime
  /*! @ingroup Utility */
  struct Exception : public std::runtime_error
  {
    explicit Exception( const std::string & what_ ) : std::runtime_error(what_) {}
    explicit Exception( const char * what_ ) : std::runtime_error(what_) {}
  };

  // ######################################################################
  //! The size type used by cereal
  /*! To ensure compatability between 32, 64, etc bit machines, we need to use
      a fixed size type instead of size_t, which may vary from machine to
      machine. */
  using size_type = uint64_t;

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
      is a third method which will elide the names when they are not used by
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
      // If we get passed an array, keep the type as is, otherwise store
      // a reference if we were passed an l value reference, else copy the value
      using Type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
                                             typename std::remove_cv<T>::type,
                                             typename std::conditional<std::is_lvalue_reference<T>::value,
                                                                       T,
                                                                       typename std::decay<T>::type>::type>::type;

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
                   size() call.
          @internal */
      NameValuePair( char const * n, T && v ) : name(n), value(std::forward<T>(v)) {}

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
  #define CEREAL_NVP_(name, value) ::cereal::make_nvp<Archive>(name, value)

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
    using PT = typename std::conditional<std::is_const<typename std::remove_pointer<T>::type>::value,
                                         const void *,
                                         void *>::type;

    BinaryData( T && d, uint64_t s ) : data(std::forward<T>(d)), size(s) {}

    PT data;       //!< pointer to beginning of data
    uint64_t size; //!< size in bytes
  };

  // ######################################################################
  namespace detail
  {
    // base classes for type checking
    class OutputArchiveBase { protected: virtual void rtti(){} };
    class InputArchiveBase { protected: virtual void rtti(){} };

    // forward decls for polymorphic support
    template <class Archive, class T> struct polymorphic_serialization_support;
    struct adl_tag;

    // used during saving pointers
    static const int32_t msb_32bit  = 0x80000000;
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
      // Store a reference if passed an lvalue reference, otherwise
      // make a copy of the data
      using Type = typename std::conditional<std::is_lvalue_reference<T>::value,
                                             T,
                                             typename std::decay<T>::type>::type;

    public:
      SizeTag( T && sz ) : size(std::forward<T>(sz)) {}

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
    using KeyType = typename std::conditional<
      std::is_lvalue_reference<Key>::value,
      Key,
      typename std::decay<Key>::type>::type;

    using ValueType = typename std::conditional<
      std::is_lvalue_reference<Value>::value,
      Value,
      typename std::decay<Value>::type>::type;

    //! Construct a MapItem from a key and a value
    /*! @internal */
    MapItem( Key && key_, Value && value_ ) : key(std::forward<Key>(key_)), value(std::forward<Value>(value_)) {}

    KeyType key;
    ValueType value;

    //! Serialize the MapItem with the NVPs "key" and "value"
    template <class Archive> inline
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive & archive)
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

  namespace detail
  {
    // ######################################################################
    //! Holds all registered version information
    struct Versions
    {
      std::unordered_map<std::size_t, std::uint32_t> mapping;
    }; // struct Versions

    //! Version information class
    /*! This is the base case for classes that have not been explicitly
        registered */
    template <class T> struct Version
    {
      static const std::uint32_t version = 0;
      // we don't need to explicitly register these types since they
      // always get a version number of 0
    };

    #ifdef CEREAL_FUTURE_EXPERIMENTAL
    // ######################################################################
    //! A class that can store any type
    /*! This is inspired by boost::any and is intended to be a very light-weight
        replacement for internal use only.

        This class is only here as a candidate for issue #46 (see github) and
        should be considered unsupported until a future version of cereal.
        */
    class Any
    {
      private:
        //! Convenience alias for decay
        template <class T>
        using ST = typename std::decay<T>::type;

        struct Base
        {
          virtual ~Base() {}
          virtual std::unique_ptr<Base> clone() const = 0;
        };

        template <class T>
        struct Derived : Base
        {
          template <class U>
          Derived( U && data ) : value( std::forward<U>( data ) ) {}

          std::unique_ptr<Base> clone() const
          {
            return std::unique_ptr<Base>( new Derived<T>( value ) );
          }

          T value;
        };

      public:
        //! Construct from any type
        template <class T> inline
        Any( T && data ) :
          itsPtr( new Derived<ST<T>>( std::forward<T>( data ) ) )
        { }

        //! Convert to any type, if possible
        /*! Attempts to perform the conversion if possible,
            otherwise throws an exception.

            @throw Exception if conversion is not possible (see get() for more info)
            @tparam The requested type to convert to */
        template <class T> inline
        operator T()
        {
          return get<ST<T>>();
        }

        Any() : itsPtr() {}
        Any( Any & other ) : itsPtr( other.clone() ) {}
        Any( Any const & other ) : itsPtr( other.clone() ) {}
        Any( Any && other ) : itsPtr( std::move( other.itsPtr ) ) {}

        Any & operator=( Any const & other )
        {
          if( itsPtr == other.itsPtr ) return *this;

          auto cloned = other.clone();
          itsPtr = std::move( cloned );

          return *this;
        }

        Any & operator=( Any && other )
        {
          if( itsPtr == other.itsPtr ) return *this;

          itsPtr = std::move( other.itsPtr );

          return *this;
        }

      protected:
        //! Get the contained type as type T
        /*! @tparam T The requested type to convert to
            @return The type converted to T
            @throw Exception if conversion is impossible */
        template <class T>
        ST<T> & get()
        {
          auto * derived = dynamic_cast<Derived<ST<T>> *>( itsPtr.get() );

          if( !derived )
            throw ::cereal::Exception( "Invalid conversion requested on ASDFA" );

          return derived->value;
        }

        //! Clones the held data if it exists
        std::unique_ptr<Base> clone() const
        {
          if( itsPtr ) return itsPtr->clone();
          else return {};
        }

      private:
        std::unique_ptr<Base> itsPtr;
    }; // struct Any
    #endif // CEREAL_FUTURE_EXPERIMENTAL
  } // namespace detail
} // namespace cereal

#endif // CEREAL_DETAILS_HELPERS_HPP_
