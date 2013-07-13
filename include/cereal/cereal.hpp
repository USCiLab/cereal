/*! \file cereal.hpp
    \brief Main cereal functionality */
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
#ifndef CEREAL_CEREAL_HPP_
#define CEREAL_CEREAL_HPP_

#include <stdexcept>
#include <type_traits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <cstdint>

#include <cereal/details/traits.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/base_class.hpp>

namespace cereal
{
  // ######################################################################
  //! An exception class thrown when things go wrong at runtime
  /*! @ingroup Utility */
  struct Exception : public std::runtime_error
  {
    Exception( const std::string& what ) : std::runtime_error(what) {}
    Exception( const char* what ) : std::runtime_error(what) {}
  };

  // ######################################################################
  //! Creates a name value pair
  /*! @relates NameValuePair
      @ingroup Utility */
  template <class T> inline
  NameValuePair<T> make_nvp( std::string const & name, T && value )
  {
    return {name.c_str(), std::forward<T>(value)};
  }

  //! Creates a name value pair
  /*! @relates NameValuePair
      @ingroup Utility */
  template <class T> inline
  NameValuePair<T> make_nvp( const char * name, T && value )
  {
    return {name, std::forward<T>(value)};
  }

  //! Creates a name value pair for the variable T with the same name as the variable
  /*! @relates NameValuePair
      @ingroup Utility */
  #define CEREAL_NVP(T) ::cereal::make_nvp(#T, T)

  // ######################################################################
  //! Convenience function to create binary data for both const and non const pointers
  /*! @param data Pointer to beginning of the data
      @param size The size in bytes of the data
      @relates BinaryData
      @ingroup Utility */
  template <class T> inline
  BinaryData<T> binary_data( T && data, size_t size )
  {
    return {std::forward<T>(data), size};
  }

  // ######################################################################
  //! Creates a size tag from some variable.
  /*! Will normally be used to serialize size (e.g. size()) information for
      variable size containers.  If you have a variable sized container,
      the very first thing it serializes should be its size, wrapped in
      a SizeTag.

      @relates SizeTag
      @ingroup Utility */
  template <class T>
  SizeTag<T> make_size_tag( T && sz )
  {
    return {std::forward<T>(sz)};
  }

  // ######################################################################
  //! Called before a type is serialized to set up any special archive state
  //! for processing some type
  /*! If designing a serializer that needs to set up any kind of special
      state or output extra information for a type, specialize this function
      for the archive type and the types that require the extra information.
      @ingroup Internal */
  template <class Archive, class T>
  void prologue( Archive & /* archive */, T const & /* data */)
  { }

  //! Called after a type is serialized to tear down any special archive state
  //! for processing some type
  /*! @ingroup Internal */
  template <class Archive, class T>
  void epilogue( Archive & /* archive */, T const & /* data */)
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
        in cereal/details/traits.hpp.
      @ingroup Internal */
  enum Flags { AllowEmptyClassElision = 1 };

  // ######################################################################
  //! Registers a specific Archive type with cereal
  /*! This registration should be done once per archive.  A good place to
      put this is immediately following the definition of your archive.
      Archive registration is only strictly necessary if you wish to
      support pointers to polymorphic data types
      @ingroup Internal */
  #define CEREAL_REGISTER_ARCHIVE(Archive)                            \
  namespace cereal { namespace detail {                               \
  template <class T>                                                  \
  typename polymorphic_serialization_support<Archive, T>::type        \
  instantiate_polymorphic_binding( T*, Archive*, adl_tag );           \
  } } // end namespaces

  //! The base output archive class
  /*! This is the base output archive for all output archives.  If you create
      a custom archive class, it should derive from this, passing itself as
      a template parameter for the ArchiveType.

      The base class provides all of the functionality necessary to
      properly forward data to the correct serialization functions.

      Individual archives should use a combination of prologue and
      epilogue functions together with specializations of serialize, save,
      and load to alter the functionality of their serialization.

      @tparam ArchiveType The archive type that derives from OutputArchive
      @tparam Flags Flags to control advanced functionality.  See the Flags
                    enum for more information.
      @ingroup Internal */
  template<class ArchiveType, std::uint32_t Flags = 0>
  class OutputArchive : public detail::OutputArchiveBase
  {
    public:
      //! Construct the output archive
      /*! @param self A pointer to the derived ArchiveType (pass this from the derived archive) */
      OutputArchive(ArchiveType * const self) : self(self), itsCurrentPointerId(1), itsCurrentPolymorphicTypeId(1)
      { }

      //! Serializes all passed in data
      template <class ... Types> inline
      ArchiveType & operator()( Types && ... args )
      {
        self->process( std::forward<Types>( args )... );
        return *self;
      }

      //! Registers a shared pointer with the archive
      /*! This function is used to track shared pointer targets to prevent
          unnecessary saves from taking place if multiple shared pointers
          point to the same data.

          @param addr The address (see shared_ptr get()) pointed to by the shared pointer
          @return A key that uniquely identifies the pointer */
      std::uint32_t registerSharedPointer( void const * addr )
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

      //! Registers a polymorphic type name with the archive
      /*! This function is used to track polymorphic types to prevent
          unnecessary saves of identifying strings used by the polymorphic
          support functionality.

          @param name The name to associate with a polymorphic type
          @return A key that uniquely identifies the polymorphic type name */
      std::uint32_t registerPolymorphicType( char const * name )
      {
        auto id = itsPolymorphicTypeMap.find( name );
        if( id == itsPolymorphicTypeMap.end() )
        {
          auto polyId = itsCurrentPolymorphicTypeId++;
          itsPolymorphicTypeMap.insert( {name, polyId} );
          return polyId | detail::msb_32bit; // mask MSB to be 1
        }
        else
          return id->second;
      }

    private:
      //! Serializes data after calling prologue, then calls epilogue
      template <class T> inline
      void process( T && head )
      {
        prologue( *self, head );
        self->processImpl( head );
        epilogue( *self, head );
      }

      //! Unwinds to process all data
      template <class T, class ... Other> inline
      void process( T && head, Other && ... tail )
      {
        self->process( std::forward<T>( head ) );
        self->process( std::forward<Other>( tail )... );
      }

      //! Serialization of a virtual_base_class wrapper
      /*! \sa virtual_base_class */
      template <class T> inline
      ArchiveType & processImpl(virtual_base_class<T> const & b)
      {
        traits::detail::base_class_id id(b.base_ptr);
        if(itsBaseClassSet.count(id) == 0)
        {
          itsBaseClassSet.insert(id);
          self->processImpl( *b.base_ptr );
        }
        return *self;
      }

      //! Serialization of a base_class wrapper
      /*! \sa base_class */
      template <class T> inline
      ArchiveType & processImpl(base_class<T> const & b)
      {
        self->processImpl( *b.base_ptr );
        return *self;
      }

      //! Member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_serialize<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T const & t)
      {
        access::member_serialize(*self, const_cast<T &>(t));
        return *self;
      }

      //! Non member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_serialize<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T const & t)
      {
        serialize(*self, const_cast<T &>(t));
        return *self;
      }

      //! Member split (save)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_save<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_member_save<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T const & t)
      {
        access::member_save(*self, t);
        return *self;
      }

      //! Non member split (save)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_save<T, ArchiveType>() ||
                              (traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_save<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T const & t)
      {
        save(*self, t);
        return *self;
      }

      //! Empty class specialization
      template <class T> inline
      typename std::enable_if<(Flags & AllowEmptyClassElision) &&
          !traits::is_output_serializable<T, ArchiveType>() && traits::is_empty_class<T>(), ArchiveType &>::type
      processImpl(T const &)
      {
        return *self;
      }

      //! No matching serialization
      template <class T> inline
      typename std::enable_if<!traits::is_specialized<T, ArchiveType>() && !traits::is_output_serializable<T, ArchiveType>() &&
        (!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !traits::is_empty_class<T>())),
        ArchiveType &>::type
      processImpl(T const &)
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
      std::unordered_map<void const *, std::uint32_t> itsSharedPointerMap;

      //! The id to be given to the next pointer
      std::uint32_t itsCurrentPointerId;

      //! Maps from polymorphic type name strings to ids
      std::unordered_map<char const *, std::uint32_t> itsPolymorphicTypeMap;

      //! The id to be given to the next polymorphic type name
      std::uint32_t itsCurrentPolymorphicTypeId;
  }; // class OutputArchive

  // ######################################################################
  //! The base input archive class
  /*! This is the base input archive for all input archives.  If you create
      a custom archive class, it should derive from this, passing itself as
      a template parameter for the ArchiveType.

      The base class provides all of the functionality necessary to
      properly forward data to the correct serialization functions.

      Individual archives should use a combination of prologue and
      epilogue functions together with specializations of serialize, save,
      and load to alter the functionality of their serialization.

      @tparam ArchiveType The archive type that derives from InputArchive
      @tparam Flags Flags to control advanced functionality.  See the Flags
                    enum for more information.
      @ingroup Internal */
  template<class ArchiveType, std::uint32_t Flags = 0>
  class InputArchive : public detail::InputArchiveBase
  {
    public:
      //! Construct the output archive
      /*! @param self A pointer to the derived ArchiveType (pass this from the derived archive) */
      InputArchive(ArchiveType * const self) : self(self) { }

      //! Serializes all passed in data
      template <class ... Types> inline
      ArchiveType & operator()( Types && ... args )
      {
        process( std::forward<Types>( args )... );
        return *self;
      }

      //! Retrieves a shared pointer given a unique key for it
      /*! This is used to retrieve a previously registered shared_ptr
          which has already been loaded.

          @param id The unique id that was serialized for the pointer
          @return A shared pointer to the data */
      std::shared_ptr<void> getSharedPointer(std::uint32_t const id)
      {
        if(id == 0) return std::shared_ptr<void>(nullptr);

        auto ptr = itsSharedPointerMap.find( id );
        if(ptr == itsSharedPointerMap.end())
          throw Exception("Error while trying to deserialize a smart pointer. Could not find id " + std::to_string(id));

        return ptr->second;
      }

      //! Registers a shared pointer to its unique identifier
      /*! After a shared pointer has been loaded for the first time, it should
          be registered with its loaded id for future references to it.

          @param id The unique identifier for the shared pointer
          @param ptr The actual shared pointer */
      void registerSharedPointer(std::uint32_t const id, std::shared_ptr<void> ptr)
      {
        std::uint32_t const stripped_id = id & ~detail::msb_32bit;
        itsSharedPointerMap.insert( {stripped_id, ptr} );
      }

      //! Retrieves the string for a polymorphic type given a unique key for it
      /*! This is used to retrieve a string previously registered during
          a polymorphic load.

          @param id The unique id that was serialized for the polymorphic type
          @return The string identifier for the tyep */
      std::string getPolymorphicName(std::uint32_t const id)
      {
        auto name = itsPolymorphicTypeMap.find( id );
        if(name == itsPolymorphicTypeMap.end())
        {
          throw Exception("Error while trying to deserialize a polymorphic pointer. Could not find type id " + std::to_string(id));
        }
        return name->second;
      }

      //! Registers a polymorphic name string to its unique identifier
      /*! After a polymorphic type has been loaded for the first time, it should
          be registered with its loaded id for future references to it.

          @param id The unique identifier for the polymorphic type
          @param name The name associated with the tyep */
      void registerPolymorphicName(std::uint32_t const id, std::string const & name)
      {
        std::uint32_t const stripped_id = id & ~detail::msb_32bit;
        itsPolymorphicTypeMap.insert( {stripped_id, name} );
      }

    private:
      //! Serializes data after calling prologue, then calls epilogue
      template <class T> inline
      void process( T && head )
      {
        prologue( *self, head );
        self->processImpl( head );
        epilogue( *self, head );
      }

      //! Unwinds to process all data
      template <class T, class ... Other> inline
      void process( T && head, Other && ... tail )
      {
        process( std::forward<T>( head ) );
        process( std::forward<Other>( tail )... );
      }

      //! Serialization of a virtual_base_class wrapper
      /*! \sa virtual_base_class */
      template <class T> inline
      ArchiveType & processImpl(virtual_base_class<T> & b)
      {
        traits::detail::base_class_id id(b.base_ptr);
        if(itsBaseClassSet.count(id) == 0)
        {
          itsBaseClassSet.insert(id);
          self->processImpl( *b.base_ptr );
        }
        return *self;
      }

      //! Serialization of a base_class wrapper
      /*! \sa base_class */
      template <class T> inline
      ArchiveType & processImpl(base_class<T> & b)
      {
        self->processImpl( *b.base_ptr );
        return *self;
      }


      //! Member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_serialize<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T & t)
      {
        access::member_serialize(*self, t);
        return *self;
      }

      //! Non member serialization
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_serialize<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T & t)
      {
        serialize(*self, t);
        return *self;
      }

      //! Member split (load)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_member_load<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_member_load<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T & t)
      {
        access::member_load(*self, t);
        return *self;
      }

      //! Non member split (load)
      template <class T> inline
      typename std::enable_if<traits::is_specialized_non_member_load<T, ArchiveType>() ||
                              (traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_load<T, ArchiveType>()),
                              ArchiveType &>::type
      processImpl(T & t)
      {
        load(*self, t);
        return *self;
      }

      //! Empty class specialization
      template <class T> inline
      typename std::enable_if<(Flags & AllowEmptyClassElision) &&
          !traits::is_input_serializable<T, ArchiveType>() && traits::is_empty_class<T>(), ArchiveType &>::type
      processImpl(T const &)
      {
        return *self;
      }

      //! No matching serialization
      template <class T> inline
      typename std::enable_if<!traits::is_specialized<T, ArchiveType>() && !traits::is_input_serializable<T, ArchiveType>() &&
        (!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !traits::is_empty_class<T>())),
        ArchiveType &>::type
      processImpl(T const &)
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

      //! Maps from pointer ids to addresses
      std::unordered_map<std::uint32_t, std::shared_ptr<void>> itsSharedPointerMap;

      //! Maps from name ids to names
      std::unordered_map<std::uint32_t, std::string> itsPolymorphicTypeMap;
  }; // class InputArchive
} // namespace cereal

// This include needs to come after things such as binary_data, make_nvp, etc
#include <cereal/types/common.hpp>

#endif // CEREAL_CEREAL_HPP_
