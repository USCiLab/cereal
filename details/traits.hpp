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
#ifndef CEREAL_DETAILS_TRAITS_HPP_
#define CEREAL_DETAILS_TRAITS_HPP_

#include <type_traits>
#include <memory>

namespace cereal
{
  namespace traits
  {
    template<typename> struct Void { typedef void type; };

    // ######################################################################
    // Member Serialize
    template<typename T, class A, typename Sfinae = void>
      struct has_member_serialize: std::false_type {};

    template<typename T, class A>
      struct has_member_serialize< T, A,
      typename Void<
        decltype( std::declval<T&>().serialize( std::declval<A&>() ) )
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
        decltype( std::declval<T&>().load( std::declval<A&>() ) )
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
        decltype( std::declval<T const &>().save( std::declval<A&>() ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    // Non Member Save
    char & save(...);
    template<typename T, typename A>
      bool constexpr has_non_member_save()
      { return std::is_void<decltype(save(std::declval<A&>(), std::declval<T&>()))>::value; };

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
    }

    template<class T>
      using is_empty_class = std::integral_constant<bool, detail::is_empty_class_impl<T>::value>;

    // ######################################################################
    //! A macro to use to restrict which types of archives your serialize function will work for.
    /*! This requires you to have a template class parameter named Archive and replaces the void return
        type for your serialize function.

        INTYPE refers to the input archive type you wish to restrict on.
        OUTTYPE refers to the output archive type you wish to restrict on.

        For example, if we want to limit a serialize to only work with binary serialization:

        @code{.cpp}
        template <class Archive>
        CEREAL_ARCHIVE_RESTRICT_SERIALIZE(BinaryInputArchive, BinaryOutputArchive)
        serialize( Archive & ar, MyCoolType & m )
        {
          ar & m;
        }
        @endcode
     */
    #define CEREAL_ARCHIVE_RESTRICT_SERIALIZE(INTYPE, OUTTYPE) \
    typename std::enable_if<std::is_same<Archive, INTYPE>::value || std::is_same<Archive, OUTTYPE>::value, void>::type
  }
}

#endif // CEREAL_DETAILS_TRAITS_HPP_
