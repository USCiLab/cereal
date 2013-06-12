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
    template<typename T, typename A> char & save(A&, T&);
    template<typename T, typename A>
      bool constexpr has_non_member_save()
      { return std::is_void<decltype(save(std::declval<A&>(), std::declval<T&>()))>::value; };

    // ######################################################################
    template <class T, class A>
      constexpr bool has_member_split()
      { return has_member_load<T, A>() && has_member_save<T, A>(); }

    // ######################################################################
    template <class T, class A>
      constexpr bool has_non_member_split()
      { return has_non_member_load<T, A>() && has_non_member_save<T, A>(); }

    // ######################################################################
    template <class T, class A>
      constexpr bool is_serializable()
      { return has_member_split<T, A>() ^ has_member_serialize<T, A>() ^ has_non_member_split<T, A>() ^ has_non_member_serialize<T,A>(); }
  }
}
