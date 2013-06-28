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
#ifndef CEREAL_CEREAL_HPP_
#define CEREAL_CEREAL_HPP_

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <cstdint>

#include <cereal/details/traits.hpp>
#include <cereal/types/virtual_base_class.hpp>
#include <cereal/types/common.hpp>

namespace cereal
{
  // ######################################################################
  //! An exception class thrown when things go wrong at runtime
  struct Exception : public std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };

  // ######################################################################
  //! For holding name value pairs
  /*! This pairs a name (some string) with some value such that an archive
      can potentially take advantage of the pairing. */
  template <class T>
  class NameValuePair
  {
    private:
      // If we get passed an RValue, we'll just make a local copy if it here
      // otherwise, we store a reference
      using DT = typename std::decay<T>::type;
      using Type = typename std::conditional<std::is_rvalue_reference<T>::value,
                                             DT,
                                            typename std::add_lvalue_reference<DT>::type>::type;
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

  //! Creates a name value pair
  template <class T> inline
  NameValuePair<T> make_nvp( std::string const & name, T && value )
  {
    return {name.c_str(), std::forward<T>(value)};
  }

  //! Creates a name value pair
  template <class T> inline
  NameValuePair<T> make_nvp( const char * name, T && value )
  {
    return {name, std::forward<T>(value)};
  }

  //! Creates a name value pair for the variable T, using the same name
  #define CEREAL_NVP(T) ::cereal::make_nvp(#T, T)

  // ######################################################################
  //! A wrapper around data that can be serialized in a binary fashion
  template <class T>
  struct BinaryData
  {
    using PT = typename std::conditional<std::is_const<typename std::remove_pointer<T>::type>::value,
                                         const void *,
                                         void *>::type;

    BinaryData( T && d, uint64_t s ) : data(d), size(s) {}

    PT data;   // pointer to beginning of data
    uint64_t size; // size in bytes
  };

  //! Convenience function to create binary data for both const and non const pointers
  /*! @param data Pointer to beginning of the data
      @param size The size in bytes of the data */
  template <class T> inline
  BinaryData<T> binary_data( T && data, size_t size )
  {
    return {std::forward<T>(data), size};
  }

  // ######################################################################
  //! Called before a type is serialized to set up any special archive state
  //! for processing some type
  template <class Archive, class T>
  void prologue( Archive & ar, T const & data )
  { }

  //! Called before a type is serialized to tear down any special archive state
  //! for processing some type
  template <class Archive, class T>
  void epilogue( Archive & ar, T const & data )
  { }

  // ######################################################################
  //! Special flags for archives
  /*! AllowEmptyClassElision
        This allows for empty classes to be serialized even if they do not provide
        a serialization function.  Classes with no data members are considered to be
        empty.  Be warned that if this is enabled and you attempt to serialize an
        empty class with improperly formed serialize or save/load functions, no
        static error will occur - the error will propogate silently and your
        intended serialization functions may not be called.  You can manually
        ensure that your classes that have custom serialization are correct
        by using the traits is_output_serializable and is_input_serializable
        in cereal/details/traits.hpp. */
  enum Flags { AllowEmptyClassElision = 1 };

  // ######################################################################
  namespace detail
  {
    // base classes for type checking
    struct OutputArchiveBase {};
    struct InputArchiveBase {};

    // forward decls for polymorphic support
    template <class Archive, class T> struct polymorphic_serialization_support;
    struct adl_tag;

    static const int32_t msb_32bit = 0x80000000;
  }

  //! Registers a specific Archive type with cereal
  /*! This registration should be done once per archive.  A good place to
      put this is immediately following the definition of your archive */
  #define CEREAL_REGISTER_ARCHIVE(Archive)                            \
  namespace cereal { namespace detail {                               \
  template <class T>                                                  \
  typename polymorphic_serialization_support<Archive, T>::type        \
  instantiate_polymorphic_binding( T*, Archive*, adl_tag );           \
  } } // end namespaces

  //! The base output archive class
  template<class ArchiveType, uint32_t Flags = 0>
  class OutputArchive : public detail::OutputArchiveBase
  {
    public:
      OutputArchive(ArchiveType * const self) : self(self), itsCurrentPointerId(1)
      { }

      //! Serializes all passed in data
      template <class ... Types> inline
      ArchiveType & operator()( Types && ... args )
      {
        process( std::forward<Types>( args )... );
        return *self;
      }

      //! Registers a pointer with the archive
      uint32_t registerSharedPointer( void * addr )
      {
        // Handle null pointers by just returning 0
        if(addr == 0) return 0;

        auto id = itsSharedPointerMap.find( addr );
        if( id == itsSharedPointerMap.end() )
        {
          auto ptrId = itsCurrentPointerId++;
          itsSharedPointerMap.insert( {addr, ptrId} );
          return ptrId | detail::msb_32bit; // mask MSB to be 1
        }
        else
          return id->second;
      }

    private:
      template <class T> inline
      void process( T && head )
      {
        prologue( *self, head );
        (*self) & head;
        epilogue( *self, head );
      }

      template <class T, class ... Other> inline
      void process( T && head, Other && ... tail )
      {
        process( std::forward<T>( head ) );
        process( std::forward<Other>( tail )... );
      }

      //! Serialization of a virtual_base_class wrapper
      /*! \sa virtual_base_class */
      template <class T> inline
      ArchiveType & operator & (virtual_base_class<T> b)
      {
        traits::detail::base_class_id id(b.base_ptr);
        if(itsBaseClassSet.count(id) == 0)
        {
          itsBaseClassSet.insert(id);
          (*self) & (*b.base_ptr);
        }
        return *self;
      }

      //! Member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_serialize<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T const & t)
      {
        access::member_serialize(*self, const_cast<T &>(t));
        return *self;
      }

      //! Non member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_serialize<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T const & t)
      {
        serialize(*self, const_cast<T &>(t));
        return *self;
      }

      //! Member split (save)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_load_save<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_member_save<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T const & t)
      {
        access::member_save(*self, t);
        return *self;
      }

      //! Non member split (save)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_load_save<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_save<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T const & t)
      {
        save(*self, t);
        return *self;
      }

      //! Empty class specialization
      template <class T> inline
      typename std::enable_if<(Flags & AllowEmptyClassElision) &&
          !traits::is_output_serializable<T, ArchiveType>() && traits::is_empty_class<T>(), ArchiveType &>::type
      operator & (T const & t)
      {
        return *self;
      }

      //! No matching serialization
      template <class T> inline
      typename std::enable_if<!traits::is_specialized<T, ArchiveType>() && !traits::is_output_serializable<T, ArchiveType>() &&
        (!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !traits::is_empty_class<T>())),
        ArchiveType &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_output_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type with an output archive.\n\n"
            "Types must either have a serialize function, or separate save/load functions (but not both).\n"
            "Serialize functions generally have the following signature:\n\n"
            "template<class Archive>\n"
            "  void serialize(Archive & ar)\n"
            "  {\n"
            "    ar( member1, member2, member3 );\n"
            "  }\n\n" );
        return *self;
      }

    private:
      ArchiveType * const self;

      //! A set of all base classes that have been serialized
      std::unordered_set<traits::detail::base_class_id, traits::detail::base_class_id_hash> itsBaseClassSet;

      //! Maps from addresses to pointer ids
      std::unordered_map<void *, std::size_t> itsSharedPointerMap;

      //! The id to be given to the next pointer
      std::size_t itsCurrentPointerId;
  }; // class OutputArchive

  // ######################################################################
  //! The base input archive class
  template<class ArchiveType, uint32_t Flags = 0>
  class InputArchive : public detail::InputArchiveBase
  {
    public:
      InputArchive(ArchiveType * const self) : self(self) { }

      //! Serializes all passed in data
      template <class ... Types> inline
      ArchiveType & operator()( Types && ... args )
      {
        process( std::forward<Types>( args )... );
        return *self;
      }

      std::shared_ptr<void> getSharedPointer(uint32_t const id)
      {
        if(id == 0) return std::shared_ptr<void>(nullptr);

        auto ptr = itsSharedPointerMap.find( id );
        if(ptr == itsSharedPointerMap.end())
        {
          throw Exception("Error while trying to deserialize a smart pointer. Could not find id " + std::to_string(id));
        }
        return ptr->second;
      }

      void registerSharedPointer(uint32_t const id, std::shared_ptr<void> ptr)
      {
        uint32_t const stripped_id = id & ~detail::msb_32bit;
        itsSharedPointerMap.insert( {stripped_id, ptr} );
      }


    private:
      template <class T> inline
      void process( T && head )
      {
        prologue( *self, head );
        (*self) & head;
        epilogue( *self, head );
      }

      template <class T, class ... Other> inline
      void process( T && head, Other && ... tail )
      {
        process( std::forward<T>( head ) );
        process( std::forward<Other>( tail )... );
      }

      //! Serialization of a virtual_base_class wrapper
      /*! \sa virtual_base_class */
      template <class T> inline
      ArchiveType & operator & (virtual_base_class<T> b)
      {
        traits::detail::base_class_id id(b.base_ptr);
        if(itsBaseClassSet.count(id) == 0)
        {
          itsBaseClassSet.insert(id);
          (*self) & (*b.base_ptr);
        }
        return *self;
      }

      //! Member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_serialize<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T && t)
      {
        access::member_serialize(*self, t);
        return *self;
      }

      //! Non member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_serialize<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T && t)
      {
        serialize(*self, std::forward<T>(t));
        return *self;
      }

      //! Member split (load)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_load_save<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_member_load<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T && t)
      {
        access::member_load(*self, t);
        return *self;
      }

      //! Non member split (load)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_load_save<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_load<T, ArchiveType>()),
                              ArchiveType &>::type
      operator & (T && t)
      {
        load(*self, std::forward<T>(t));
        return *self;
      }

      //! Empty class specialization
      template <class T> inline
      typename std::enable_if<(Flags & AllowEmptyClassElision) &&
          !traits::is_input_serializable<T, ArchiveType>() && traits::is_empty_class<T>(), ArchiveType &>::type
      operator & (T const & t)
      {
        return *self;
      }

      //! No matching serialization
      template <class T> inline
      typename std::enable_if<!traits::is_specialized<T, ArchiveType>() && !traits::is_input_serializable<T, ArchiveType>() &&
        (!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !traits::is_empty_class<T>())),
        ArchiveType &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_output_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type with an output archive.\n\n"
            "Types must either have a serialize function, or separate save/load functions (but not both).\n"
            "Serialize functions generally have the following signature:\n\n"
            "template<class Archive>\n"
            "  void serialize(Archive & ar)\n"
            "  {\n"
            "    ar( member1, member2, member3 );\n"
            "  }\n\n" );
        return *self;
      }

    private:
      ArchiveType * const self;

      //! A set of all base classes that have been serialized
      std::unordered_set<traits::detail::base_class_id, traits::detail::base_class_id_hash> itsBaseClassSet;

      //! Maps from addresses to pointer ids
      std::unordered_map<std::size_t, std::shared_ptr<void>> itsSharedPointerMap;
  }; // class InputArchive
} // namespace cereal

#endif // CEREAL_CEREAL_HPP_
