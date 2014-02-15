/*! \file polymorphic_impl.hpp
    \brief Internal polymorphism support
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

/* This code is heavily inspired by the boost serialization implementation by the following authors

   (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
   Use, modification and distribution is subject to the Boost Software
   License, Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt)

    See http://www.boost.org for updates, documentation, and revision history.

   (C) Copyright 2006 David Abrahams - http://www.boost.org.

   See /boost/serialization/export.hpp and /boost/archive/detail/register_archive.hpp for their
   implementation.
*/
#ifndef CEREAL_DETAILS_POLYMORPHIC_IMPL_HPP_
#define CEREAL_DETAILS_POLYMORPHIC_IMPL_HPP_

#include <cereal/details/static_object.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <functional>
#include <typeindex>
#include <map>

//! Binds a polymorhic type to all registered archives
/*! This binds a polymorphic type to all registered archives that
    have been registered with CEREAL_REGISTER_ARCHIVE.  This must be called
    after all archives are registered (usually after the archives themselves
    have been included). */
#define CEREAL_BIND_TO_ARCHIVES(T)                           \
    namespace cereal {                                       \
    namespace detail {                                       \
    template<>                                               \
    struct init_binding<T> {                                 \
        static bind_to_archives<T> const & b;                \
    };                                                       \
    bind_to_archives<T> const & init_binding<T>::b =         \
        ::cereal::detail::StaticObject<                      \
            bind_to_archives<T>                              \
        >::getInstance().bind();                             \
    }} // end namespaces

namespace cereal
{
  namespace detail
  {
    //! Binds a compile time type with a user defined string
    template <class T>
    struct binding_name {};

    //! A structure holding a map from type_indices to output serializer functions
    /*! A static object of this map should be created for each registered archive
        type, containing entries for every registered type that describe how to
        properly cast the type to its real type in polymorphic scenarios for
        shared_ptr, weak_ptr, and unique_ptr. */
    template <class Archive>
    struct OutputBindingMap
    {
      //! A serializer function
      /*! Serializer functions return nothing and take an archive as
          their first parameter (will be cast properly inside the function,
          and a pointer to actual data (contents of smart_ptr's get() function)
          as their second parameter */
      typedef std::function<void(void*, void const *)> Serializer;

      //! Struct containing the serializer functions for all pointer types
      struct Serializers
      {
        Serializer shared_ptr, //!< Serializer function for shared/weak pointers
                   unique_ptr; //!< Serializer function for unique pointers
      };

      //! A map of serializers for pointers of all registered types
      std::map<std::type_index, Serializers> map;
    };

    //! An empty noop deleter
    template<class T> struct EmptyDeleter { void operator()(T *) const {} };

    //! A structure holding a map from type name strings to input serializer functions
    /*! A static object of this map should be created for each registered archive
        type, containing entries for every registered type that describe how to
        properly cast the type to its real type in polymorphic scenarios for
        shared_ptr, weak_ptr, and unique_ptr. */
    template <class Archive>
    struct InputBindingMap
    {
      //! Shared ptr serializer function
      /*! Serializer functions return nothing and take an archive as
          their first parameter (will be cast properly inside the function,
          and a shared_ptr (or unique_ptr for the unique case) of any base
          type.  Internally it will properly be loaded and cast to the
          correct type. */
      typedef std::function<void(void*, std::shared_ptr<void> & )> SharedSerializer;
      //! Unique ptr serializer function
      typedef std::function<void(void*, std::unique_ptr<void, EmptyDeleter<void>> & )> UniqueSerializer;

      //! Struct containing the serializer functions for all pointer types
      struct Serializers
      {
        SharedSerializer shared_ptr; //!< Serializer function for shared/weak pointers
        UniqueSerializer unique_ptr; //!< Serializer function for unique pointers
      };

      //! A map of serializers for pointers of all registered types
      std::map<std::string, Serializers> map;
    };

    // forward decls for archives from cereal.hpp
    struct InputArchiveBase;
    struct OutputArchiveBase;

    //! Creates a binding (map entry) between an input archive type and a polymorphic type
    /*! Bindings are made when types are registered, assuming that at least one
        archive has already been registered.  When this struct is created,
        it will insert (at run time) an entry into a map that properly handles
        casting for serializing polymorphic objects */
    template <class Archive, class T> struct InputBindingCreator
    {
      //! Initialize the binding
      InputBindingCreator()
      {
        typename InputBindingMap<Archive>::Serializers serializers;

        serializers.shared_ptr =
          [](void * arptr, std::shared_ptr<void> & dptr)
          {
            Archive & ar = *static_cast<Archive*>(arptr);
            std::shared_ptr<T> ptr;

            ar( _CEREAL_NVP("ptr_wrapper", ::cereal::memory_detail::make_ptr_wrapper(ptr)) );

            dptr = ptr;
          };

        serializers.unique_ptr =
          [](void * arptr, std::unique_ptr<void, EmptyDeleter<void>> & dptr)
          {
            Archive & ar = *static_cast<Archive*>(arptr);
            std::unique_ptr<T> ptr;

            ar( _CEREAL_NVP("ptr_wrapper", ::cereal::memory_detail::make_ptr_wrapper(ptr)) );

            dptr.reset(ptr.release());
          };

        StaticObject<InputBindingMap<Archive>>::getInstance().map.insert( { std::string(binding_name<T>::name()), serializers } );
      }
    };

    //! Creates a binding (map entry) between an output archive type and a polymorphic type
    /*! Bindings are made when types are registered, assuming that at least one
        archive has already been registered.  When this struct is created,
        it will insert (at run time) an entry into a map that properly handles
        casting for serializing polymorphic objects */
    template <class Archive, class T> struct OutputBindingCreator
    {
      //! Writes appropriate metadata to the archive for this polymorphic type
      static void writeMetadata(Archive & ar)
      {
        // Register the polymorphic type name with the archive, and get the id
        char const * name = binding_name<T>::name();
        std::uint32_t id = ar.registerPolymorphicType(name);

        // Serialize the id
        ar( _CEREAL_NVP("polymorphic_id", id) );

        // If the msb of the id is 1, then the type name is new, and we should serialize it
        if( id & detail::msb_32bit )
        {
          std::string namestring(name);
          ar( _CEREAL_NVP("polymorphic_name", namestring) );
        }
      }

      //! Initialize the binding
      OutputBindingCreator()
      {
        typename OutputBindingMap<Archive>::Serializers serializers;

        serializers.shared_ptr =
          [&](void * arptr, void const * dptr)
          {
            Archive & ar = *static_cast<Archive*>(arptr);

            writeMetadata(ar);

            #ifdef _LIBCPP_VERSION
            // libc++ needs this hacky workaround, see http://llvm.org/bugs/show_bug.cgi?id=18843
            std::shared_ptr<T const> const ptr(
                std::const_pointer_cast<T const>(
                  std::shared_ptr<T>(static_cast<T *>(const_cast<void *>(dptr)), EmptyDeleter<T>())));
            #else // NOT _LIBCPP_VERSION
            std::shared_ptr<T const> const ptr(static_cast<T const *>(dptr), EmptyDeleter<T const>());
            #endif // _LIBCPP_VERSION

            ar( _CEREAL_NVP("ptr_wrapper", memory_detail::make_ptr_wrapper(ptr)) );
          };

        serializers.unique_ptr =
          [&](void * arptr, void const * dptr)
          {
            Archive & ar = *static_cast<Archive*>(arptr);

            writeMetadata(ar);

            std::unique_ptr<T const, EmptyDeleter<T const>> const ptr(static_cast<T const *>(dptr));

            ar( _CEREAL_NVP("ptr_wrapper", memory_detail::make_ptr_wrapper(ptr)) );
          };

        StaticObject<OutputBindingMap<Archive>>::getInstance().map.insert( {std::type_index(typeid(T)), serializers} );
      }
    };

    //! Used to help out argument dependent lookup for finding potential overloads
    //! of instantiate_polymorphic_binding
    struct adl_tag {};

    //! Causes the static object bindings between an archive type and a serializable type T
    template <class Archive, class T>
    struct create_bindings
    {
      static const InputBindingCreator<Archive, T> &
      load(std::true_type)
      {
        return cereal::detail::StaticObject<InputBindingCreator<Archive, T>>::getInstance();
      }

      static const OutputBindingCreator<Archive, T> &
      save(std::true_type)
      {
        return cereal::detail::StaticObject<OutputBindingCreator<Archive, T>>::getInstance();
      }

      inline static void load(std::false_type) {}
      inline static void save(std::false_type) {}
    };

    //! When specialized, causes the compiler to instantiate its parameter
    template <void(*)()>
    struct instantiate_function {};

    /*! This struct is used as the return type of instantiate_polymorphic_binding
        for specific Archive types.  When the compiler looks for overloads of
        instantiate_polymorphic_binding, it will be forced to instantiate this
        struct during overload resolution, even though it will not be part of a valid
        overload */
    template <class Archive, class T>
    struct polymorphic_serialization_support
    {
#ifdef _MSC_VER
      //! Creates the appropriate bindings depending on whether the archive supports
      //! saving or loading
      virtual void instantiate();
#else // NOT _MSC_VER
      //! Creates the appropriate bindings depending on whether the archive supports
      //! saving or loading
      static void instantiate();
      //! This typedef causes the compiler to instantiate this static function
      typedef instantiate_function<instantiate> unused;
#endif // _MSC_VER
    };

    // instantiate implementation
    template <class Archive, class T>
    void polymorphic_serialization_support<Archive,T>::instantiate()
    {
      create_bindings<Archive,T>::save( std::is_base_of<detail::OutputArchiveBase, Archive>() );

      create_bindings<Archive,T>::load( std::is_base_of<detail::InputArchiveBase, Archive>() );
    }

    //! Begins the binding process of a type to all registered archives
    /*! Archives need to be registered prior to this struct being instantiated via
        the CEREAL_REGISTER_ARCHIVE macro.  Overload resolution will then force
        several static objects to be made that allow us to bind together all
        registered archive types with the parameter type T. */
    template <class T>
    struct bind_to_archives
    {
      //! Binding for non abstract types
      void bind(std::false_type) const
	    {
		    instantiate_polymorphic_binding((T*) 0, 0, adl_tag{});
      }

      //! Binding for abstract types
      void bind(std::true_type) const
      { }

      //! Binds the type T to all registered archives
      /*! If T is abstract, we will not serialize it and thus
          do not need to make a binding */
      bind_to_archives const & bind() const
      {
        static_assert( std::is_polymorphic<T>::value,
                       "Attempting to register non polymorphic type" );
        bind( std::is_abstract<T>() );
        return *this;
      }
    };

    //! Used to hide the static object used to bind T to registered archives
    template <class T>
    struct init_binding;

    //! Base case overload for instantiation
    /*! This will end up always being the best overload due to the second
        parameter always being passed as an int.  All other overloads will
        accept pointers to archive types and have lower precedence than int.

        Since the compiler needs to check all possible overloads, the
        other overloads created via CEREAL_REGISTER_ARCHIVE, which will have
        lower precedence due to requring a conversion from int to (Archive*),
        will cause their return types to be instantiated through the static object
        mechanisms even though they are never called.

        See the documentation for the other functions to try and understand this */
    template <class T>
    void instantiate_polymorphic_binding( T*, int, adl_tag ) {}
  } // namespace detail
} // namespace cereal

#endif // CEREAL_DETAILS_POLYMORPHIC_IMPL_HPP_
