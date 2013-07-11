/*! \file traits.hpp
    \brief Internal type trait support
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
#ifndef CEREAL_DETAILS_TRAITS_HPP_
#define CEREAL_DETAILS_TRAITS_HPP_

#include <type_traits>
#include <typeindex>
#include <memory>

#include <cereal/access.hpp>

namespace cereal
{
  namespace traits
  {
    template<typename> struct Void { typedef void type; };

    // ######################################################################
    // Member load_and_allocate
    template<typename T, typename A>
      bool constexpr has_member_load_and_allocate()
      { return std::is_same<decltype( access::load_and_allocate<T>( std::declval<A&>() ) ), T*>::value; }

    // ######################################################################
    // Non Member load_and_allocate
    template<typename T, typename A>
      bool constexpr has_non_member_load_and_allocate()
      { return std::is_same<decltype( LoadAndAllocate<T>::load_and_allocate( std::declval<A&>() ) ), T*>::value; }

    // ######################################################################
    // Has either a member or non member allocate
    template<typename T, typename A>
      bool constexpr has_load_and_allocate()
      { return has_member_load_and_allocate<T, A>() || has_non_member_load_and_allocate<T, A>(); }

    // ######################################################################
    // Member Serialize
    template<typename T, class A, typename Sfinae = void>
      struct has_member_serialize: std::false_type {};

    template<typename T, class A>
      struct has_member_serialize< T, A,
      typename Void<
        decltype( access::member_serialize(std::declval<A&>(), std::declval<T&>() ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    // Non Member Serialize
    char & serialize(...);
    template<typename T, typename A>
      bool constexpr has_non_member_serialize()
      { return std::is_void<decltype(serialize(std::declval<A&>(), std::declval<T&>()))>::value; };

    // ######################################################################
    // Member Load
    template<typename T, class A, typename Sfinae = void>
      struct has_member_load: std::false_type {};

    template<typename T, class A>
      struct has_member_load< T, A,
      typename Void<
        decltype( access::member_load(std::declval<A&>(), std::declval<T&>() ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    // Non Member Load
    char & load(...);
    template<typename T, typename A>
      bool constexpr has_non_member_load()
      { return std::is_void<decltype(load(std::declval<A&>(), std::declval<T&>()))>::value; };

    // ######################################################################
    // Member Save
    template<typename T, class A, typename Sfinae = void>
      struct has_member_save: std::false_type {};

    template<typename T, class A>
      struct has_member_save< T, A,
      typename Void<
        decltype( access::member_save(std::declval<A&>(), std::declval<T const &>() ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    // Non-const Member Save
    namespace detail
    {
      // Detection of any (const or non const) member save
      template<typename T, class A, typename Sfinae = void>
        struct has_member_save_any: std::false_type {};

      template<typename T, class A>
        struct has_member_save_any< T, A,
        typename Void<
          decltype( access::non_const_member_save(std::declval<A&>(), std::declval<typename std::remove_const<T>::type &>() ) )
          >::type
          >: std::true_type {};
    }

    // Returns true if we detect a member save function that is not const
    template <class T, class A>
    constexpr bool is_non_const_member_save()
    {
      return !has_member_save<T, A>() && detail::has_member_save_any<T, A>();
    }

    // ######################################################################
    // Non Member Save
    char & save(...);
    template<typename T, typename A>
      bool constexpr has_non_member_save()
      { return std::is_void<decltype(save(std::declval<A&>(), std::declval<T const &>()))>::value; }

    // ######################################################################
    // Non-const Non member Save
    namespace detail
    {
      template<typename T, typename A>
        bool constexpr has_non_member_save_any()
        { return std::is_void<decltype(save(std::declval<A&>(), std::declval<typename std::remove_const<T>::type &>()))>::value; }
    }

    // Returns true if we detect a non-member save function that is not const
    template<typename T, typename A>
      bool constexpr is_non_const_non_member_save()
      { return !has_non_member_save<T, A>() && detail::has_non_member_save_any<T, A>(); }

    // ######################################################################
    // Returns true if we have an invalid save function (non const)
    template <class T, class A>
    bool constexpr has_non_const_save()
    { return is_non_const_member_save<T, A>() || is_non_const_non_member_save<T, A>(); }

    // ######################################################################
    template <class T, class InputArchive, class OutputArchive>
      constexpr bool has_member_split()
      { return has_member_load<T, InputArchive>() && has_member_save<T, OutputArchive>(); }

    // ######################################################################
    template <class T, class InputArchive, class OutputArchive>
      constexpr bool has_non_member_split()
      { return has_non_member_load<T, InputArchive>() && has_non_member_save<T, OutputArchive>(); }

    // ######################################################################
    template <class T, class OutputArchive>
      constexpr bool is_output_serializable()
      {
        static_assert( !has_non_const_save<T, OutputArchive>(),
                       "cereal detected a non const save. \n "
                       "save functions must either be const member functions or accept const type aguments if non-member" );

        return
          has_member_save<T, OutputArchive>() ^
          has_non_member_save<T, OutputArchive>() ^
          has_member_serialize<T, OutputArchive>() ^
          has_non_member_serialize<T, OutputArchive>();
      }

    // ######################################################################
    template <class T, class InputArchive>
      constexpr bool is_input_serializable()
      {
        return
          has_member_load<T, InputArchive>() ^
          has_non_member_load<T, InputArchive>() ^
          has_member_serialize<T, InputArchive>() ^
          has_non_member_serialize<T, InputArchive>();
      }

    // ######################################################################

    namespace detail
    {
      template <class T, class A>
      constexpr auto is_specialized_member_serialize() -> bool
      { return !std::is_base_of<std::false_type, specialize<A, T, specialization::member_serialize>>(); }

      template <class T, class A>
      constexpr auto is_specialized_member_load_save() -> bool
      { return !std::is_base_of<std::false_type, specialize<A, T, specialization::member_load_save>>(); }

      template <class T, class A>
      constexpr auto is_specialized_non_member_serialize() -> bool
      { return !std::is_base_of<std::false_type, specialize<A, T, specialization::non_member_serialize>>(); }

      template <class T, class A>
      constexpr auto is_specialized_non_member_load_save() -> bool
      { return !std::is_base_of<std::false_type, specialize<A, T, specialization::non_member_load_save>>(); }

      // Considered an error if specialization exists for more than one type
      template <class T, class A>
      constexpr auto is_specialized_error() -> bool
      {
        return (is_specialized_member_serialize<T, A>() +
                is_specialized_member_load_save<T, A>() +
                is_specialized_non_member_serialize<T, A>() +
                is_specialized_non_member_load_save<T, A>()) <= 1;
      }
    } // namespace detail

    template <class T, class A>
    constexpr auto is_specialized() -> bool
    {
      static_assert(detail::is_specialized_error<T, A>(), "More than one explicit specialization detected for type.");
      return detail::is_specialized_member_serialize<T, A>() ||
             detail::is_specialized_member_load_save<T, A>() ||
             detail::is_specialized_non_member_serialize<T, A>() ||
             detail::is_specialized_non_member_load_save<T, A>();
    }

    template <class T, class A>
    constexpr auto is_specialized_member_serialize() -> bool
    {
      static_assert( (is_specialized<T, A>() && detail::is_specialized_member_serialize<T, A>() && has_member_serialize<T, A>())
                     || !(is_specialized<T, A>() && detail::is_specialized_member_serialize<T, A>()),
                     "cereal detected member serialization specialization but no member serialize function" );
      return is_specialized<T, A>() && detail::is_specialized_member_serialize<T, A>();
    }

    template <class T, class A>
    constexpr auto is_specialized_member_load() -> bool
    {
      static_assert( (is_specialized<T, A>() && detail::is_specialized_member_load_save<T, A>() && has_member_load<T, A>())
                     || !(is_specialized<T, A>() && detail::is_specialized_member_load_save<T, A>()),
                     "cereal detected member load specialization but no member load function" );
      return is_specialized<T, A>() && detail::is_specialized_member_load_save<T, A>();
    }

    template <class T, class A>
    constexpr auto is_specialized_member_save() -> bool
    {
      static_assert( (is_specialized<T, A>() && detail::is_specialized_member_load_save<T, A>() && has_member_save<T, A>())
                     || !(is_specialized<T, A>() && detail::is_specialized_member_load_save<T, A>()),
                     "cereal detected member save specialization but no member save function" );
      return is_specialized<T, A>() && detail::is_specialized_member_load_save<T, A>();
    }

    template <class T, class A>
    constexpr auto is_specialized_non_member_serialize() -> bool
    {
      static_assert( (is_specialized<T, A>() && detail::is_specialized_non_member_serialize<T, A>() && has_non_member_serialize<T, A>())
                     || !(is_specialized<T, A>() && detail::is_specialized_non_member_serialize<T, A>()),
                     "cereal detected non-member serialization specialization but no non-member serialize function" );
      return is_specialized<T, A>() && detail::is_specialized_non_member_serialize<T, A>();
    }

    template <class T, class A>
    constexpr auto is_specialized_non_member_load() -> bool
    {
      static_assert( (is_specialized<T, A>() && detail::is_specialized_non_member_load_save<T, A>() && has_non_member_load<T, A>())
                     || !(is_specialized<T, A>() && detail::is_specialized_non_member_load_save<T, A>()),
                     "cereal detected non-member load specialization but no non-member load function" );
      return is_specialized<T, A>() && detail::is_specialized_non_member_load_save<T, A>();
    }

    template <class T, class A>
    constexpr auto is_specialized_non_member_save() -> bool
    {
      static_assert( (is_specialized<T, A>() && detail::is_specialized_non_member_load_save<T, A>() && has_non_member_save<T, A>())
                     || !(is_specialized<T, A>() && detail::is_specialized_non_member_load_save<T, A>()),
                     "cereal detected non-member save specialization but no non-member save function" );
      return is_specialized<T, A>() && detail::is_specialized_non_member_load_save<T, A>();
    }

    // ######################################################################
    template <class T>
    constexpr size_t sizeof_array( size_t rank = std::rank<T>::value )
    {
      return rank == 0 ? 1 : std::extent<T>::value * sizeof_array<typename std::remove_extent<T>::type>( rank - 1 );
    }

    // ######################################################################
    namespace detail
    {
      template <class T, typename Enable = void>
        struct is_empty_class_impl
        { static constexpr bool value = false; };

      template <class T>
        struct is_empty_class_impl<T, typename std::enable_if<std::is_class<T>::value>::type>
        {
          struct S : T
          { uint8_t t; };

          static constexpr bool value = sizeof(S) == sizeof(uint8_t);
        };

      struct base_class_id
      {
        template<class T>
          base_class_id(T const * const t) :
          type(typeid(T)),
          ptr(t),
          hash(std::hash<std::type_index>()(typeid(T)) ^ (std::hash<void const *>()(t) << 1))
          { }

          bool operator==(base_class_id const & other) const
          { return (type == other.type) && (ptr == other.ptr); }

          std::type_index type;
          void const * ptr;
          size_t hash;
      };
      struct base_class_id_hash { size_t operator()(base_class_id const & id) const { return id.hash; }  };
    }

    template<class T>
      using is_empty_class = std::integral_constant<bool, detail::is_empty_class_impl<T>::value>;

    // ######################################################################
    //! A macro to use to restrict which types of archives your function will work for.
    /*! This requires you to have a template class parameter named Archive and replaces the void return
        type for your function.

        INTYPE refers to the input archive type you wish to restrict on.
        OUTTYPE refers to the output archive type you wish to restrict on.

        For example, if we want to limit a serialize to only work with binary serialization:

        @code{.cpp}
        template <class Archive>
        CEREAL_ARCHIVE_RESTRICT(BinaryInputArchive, BinaryOutputArchive)
        serialize( Archive & ar, MyCoolType & m )
        {
          ar & m;
        }
        @endcode

        If you need to do more restrictions in your enable_if, you will need to do this by hand.
     */
    #define CEREAL_ARCHIVE_RESTRICT(INTYPE, OUTTYPE) \
    typename std::enable_if<std::is_same<Archive, INTYPE>::value || std::is_same<Archive, OUTTYPE>::value, void>::type
  } // namespace traits

  namespace detail
  {
    template <class T, class A, bool Member = traits::has_member_load_and_allocate<T, A>(), bool NonMember = traits::has_non_member_load_and_allocate<T, A>()>
    struct Load
    {
      static_assert( !sizeof(T), "Cereal detected both member and non member load_and_allocate functions!" );
      static T * load_andor_allocate( A & ar )
      { return nullptr; }
    };

    template <class T, class A>
    struct Load<T, A, false, false>
    {
      static_assert( std::is_default_constructible<T>::value,
                     "Trying to serialize a an object with no default constructor.\n\n"
                     "Types must either be default constructible or define either a member or non member Construct function.\n"
                     "Construct functions generally have the signature:\n\n"
                     "template <class Archive>\n"
                     "static T * load_and_allocate(Archive & ar)\n"
                     "{\n"
                     "  var a;\n"
                     "  ar & a\n"
                     "  return new T(a);\n"
                     "}\n\n" );
      static T * load_andor_allocate( A & ar )
      { return new T(); }
    };

    template <class T, class A>
    struct Load<T, A, true, false>
    {
      static T * load_andor_allocate( A & ar )
      {
        return access::load_and_allocate<T>( ar );
      }
    };

    template <class T, class A>
    struct Load<T, A, false, true>
    {
      static T * load_andor_allocate( A & ar )
      {
        return LoadAndAllocate<T>::load_and_allocate( ar );
      }
    };
  } // namespace detail
} // namespace cereal

#endif // CEREAL_DETAILS_TRAITS_HPP_
