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
    template<typename T, typename A> char & serialize(A&, T&);
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
    template<typename T, typename A> char & load(A&, T&);
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
        decltype( std::declval<T&>().save( std::declval<A&>() ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    // Non Member Save
    template<typename T, typename A> char & save(A&, T const &);
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

    constexpr std::false_type is_smart_ptr(...)
    {
      return {};
    }

    template <class T, class D>
    constexpr std::true_type is_smart_ptr( std::unique_ptr<T, D> const & p )
    {
      return {};
    }

    template <class T>
    constexpr std::true_type is_smart_ptr( std::shared_ptr<T> const & p )
    {
      return {};
    }

    //! Returns true if the type T is a pointer or smart pointer (in std library)
    template <class T>
    constexpr bool is_any_pointer()
    {
      return std::is_pointer<T>() || std::is_same<std::true_type, decltype(is_smart_ptr( std::declval<T&>() ))>::value;
    }
  }
}

#endif // CEREAL_DETAILS_TRAITS_HPP_
