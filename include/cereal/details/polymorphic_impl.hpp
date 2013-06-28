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
            bind_to_archives<T >                             \
        >::getInstance().bind();                             \
    }} // end namespaces

namespace cereal
{
  namespace detail
  {
    //! Binds a compile time type with a user defined string
    template <class T>
    struct binding_name {};

    template <class Archive>
    struct OutputBindingMap
    {
      //! A serializer function
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

    struct InputArchiveBase;
    struct OutputArchiveBase;

    template <class Archive, class T> struct InputBinding 
    {
    };

    template <class Archive, class T> struct OutputBinding
    {
      OutputBinding( )
      {
        typename OutputBindingMap<Archive>::Serializers serializers;

        serializers.shared_ptr =
          [](void * arptr, void const * dptr)
          {
            Archive & ar = *static_cast<Archive*>(arptr);
            std::shared_ptr<T const> const ptr(static_cast<T const *>(dptr), EmptyDeleter<T const>());

            ar( detail::make_ptr_wrapper(ptr) );
          };

        serializers.unique_ptr =
          [](void * arptr, void const * dptr)
          {
            Archive & ar = *static_cast<Archive*>(arptr);
            std::unique_ptr<T const, EmptyDeleter<T const>> const ptr(static_cast<T const *>(dptr));

            ar( detail::make_ptr_wrapper(ptr) );
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
        static const InputBinding<Archive, T> &
        load(std::true_type)
        {
          return cereal::detail::StaticObject<InputBinding<Archive, T>>::getInstance();
        }

        static const OutputBinding<Archive, T> &
        save(std::true_type)
        {
          return cereal::detail::StaticObject<OutputBinding<Archive, T>>::getInstance();
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
      //! Creates the appropriate bindings depending on whether the archive supports
      //! saving or loading
      static void instantiate();
      //! This typedef causes the compiler to instantiate this static function
      typedef instantiate_function<instantiate> unused;
    };

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
        instantiate_polymorphic_binding( (T*)0, 0, adl_tag{} );
      }

      //! Binding for abstract types
      void bind(std::true_type) const
      { }

      //! Binds the type T to all registered archives
      /*! If T is abstract, we will not serialize it and thus
          do not need to make a binding */
      bind_to_archives const & bind() const
      {
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
    void instantiate_polymorphic_binding( T*, int, adl_tag ) {};
  } // namespace detail
} // namespace cereal

#endif // CEREAL_DETAILS_POLYMORPHIC_IMPL_HPP_
