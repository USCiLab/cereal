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
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <cstddef>

#include <cereal/details/traits.hpp>

namespace cereal
{
  static const int32_t msb_32bit = 0x80000000;

  //! An exception class thrown when things go wrong at runtime
  struct Exception : public std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };

  //! For holding name value pairs
  template <class T>
  struct NameValuePair
  {
    NameValuePair( std::string const & n, T & v ) : name(n), value(v) {}

    std::string name;
    T & value;
  };

  //! Creates a name value pair
  template <class T> inline
  NameValuePair<T> make_nvp( std::string const & name, T & value )
  {
    return {name, value};
  }

  //! Creates a name value pair for the variable T, using the same name
  #define CEREAL_NVP(T) ::cereal::make_nvp(#T, T)

  enum Flags { AllowEmptyClassElision = 1 };

  template<class Base>
    struct base_class
    {
      template<class Derived>
        base_class(Derived const * derived) :
          base_ptr(const_cast<Base*>(static_cast<Base const *>(derived)))
      { }

        Base * base_ptr;
    };

  // ######################################################################
  //! The base output archive class
  template<class ArchiveType, uint32_t Flags = 0>
  class OutputArchive
  {
    public:
      OutputArchive(ArchiveType * const self) : self(self), itsCurrentPointerId(0)
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
        auto id = itsSharedPointerMap.find( addr );
        if( id == itsSharedPointerMap.end() )
        {
          auto ptrId = itsCurrentPointerId++;
          itsSharedPointerMap.insert( {addr, ptrId} );
          return ptrId | msb_32bit; // mask MSB to be 1
        }
        else
          return id->second;
      }

    private:
      template <class T> inline
      void process( T && head )
      {
        (*self) & head;
      }

      template <class T, class ... Other> inline
      void process( T && head, Other && ... tail )
      {
        (*self) & head;
        process( std::forward<Other>( tail )... );
      }

      //! Serialization of a base_class wrapper
      /*! \sa base_class */
      template <class T> inline
      ArchiveType & operator & (base_class<T> b)
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
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        access::member_serialize(*self, const_cast<T &>(t));
        return *self;
      }

      //! Non member serialization
      template <class T> inline
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        serialize(*self, const_cast<T &>(t));
        return *self;
      }

      //! Member split (save)
      template <class T> inline
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        access::member_save(*self, t);
        return *self;
      }

      //! Non member split (save)
      template <class T> inline
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        save(*self, t);
        return *self;
      }

      //! Non member split (save) non-const version
      template <class T> inline
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T & t)
      {
        save(*self, t);
        return *self;
      }

      template <class T> inline
      typename std::enable_if<(Flags & AllowEmptyClassElision) &&
          !traits::is_output_serializable<T, ArchiveType>() && traits::is_empty_class<T>(), ArchiveType &>::type
      operator & (T const & t)
      {
        return *self;
      }

      //! No matching serialization
      template <class T> inline
      typename std::enable_if<!traits::is_output_serializable<T, ArchiveType>() &&
        (!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !traits::is_empty_class<T>())),
        ArchiveType &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_output_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type with an output archive.\n\n"
            "Types must either have a serialize function, or separate save/load functions (but not both).\n"
            "Serialize functions generally have the following signature:\n\n"
            "template<class Archive>\n"
            "  void serialize(int & ar)\n"
            "  {\n"
            "    ar & member1 & member2 & member3;\n"
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
  class InputArchive
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
        auto ptr = itsSharedPointerMap.find( id );
        if(ptr == itsSharedPointerMap.end())
        {
          throw Exception("Error while trying to deserialize a smart pointer. Could not find id " + std::to_string(id));
        }
        return ptr->second;
      }

      void registerSharedPointer(uint32_t const id, std::shared_ptr<void> ptr)
      {
        uint32_t const stripped_id = id & ~msb_32bit;
        itsSharedPointerMap.insert( {stripped_id, ptr} );
      }


    private:
      template <class T> inline
      void process( T && head )
      {
        (*self) & head;
      }

      template <class T, class ... Other> inline
      void process( T && head, Other && ... tail )
      {
        (*self) & head;
        process( std::forward<Other>( tail )... );
      }

      //! Serialization of a base_class wrapper
      /*! \sa base_class */
      template <class T> inline
      ArchiveType & operator & (base_class<T> b)
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
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        access::member_serialize(*self, t);
        return *self;
      }

      //! Non member serialization
      template <class T> inline
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        serialize(*self, std::forward<T>(t));
        return *self;
      }

      //! Member split (load)
      template <class T> inline
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_member_load<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        access::member_load(*self, t);
        return *self;
      }

      //! Non member split (load)
      template <class T> inline
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_load<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        load(*self, std::forward<T>(t));
        return *self;
      }

      template <class T> inline
      typename std::enable_if<(Flags & AllowEmptyClassElision) &&
          !traits::is_input_serializable<T, ArchiveType>() && traits::is_empty_class<T>(), ArchiveType &>::type
      operator & (T const & t)
      {
        return *self;
      }

      //! No matching serialization
      template <class T> inline
      typename std::enable_if<!traits::is_input_serializable<T, ArchiveType>() &&
        (!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !traits::is_empty_class<T>())),
        ArchiveType &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_output_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type with an output archive.\n\n"
            "Types must either have a serialize function, or separate save/load functions (but not both).\n"
            "Serialize functions generally have the following signature:\n\n"
            "template<class Archive>\n"
            "  void serialize(int & ar)\n"
            "  {\n"
            "    ar & member1 & member2 & member3;\n"
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
