namespace cereal
{
  struct BogusType {};

  //template<class X, class Y, class Z>
  //  BogusType serialize(X&,Y&,Z&);
  void load();
  void save();

  namespace traits
  {
    template<typename> struct Void { typedef void type; };

    // ######################################################################
    template<typename T, class A, typename Sfinae = void>
      struct has_member_serialize: std::false_type {};

    template<typename T, class A>
      struct has_member_serialize< T, A,
      typename Void<
        decltype( std::declval<T&>().serialize( std::declval<A&>(), 0 ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    template<typename T, class A, typename Sfinae = void>
      struct has_non_member_serialize: std::false_type {};
    
    template<typename T, class A>
      struct has_non_member_serialize< T, A,
      typename Void<
        typename std::enable_if<!
          std::is_same<decltype(serialize( std::declval<A&>(), std::declval<T&>(), 0 )), BogusType>::value, void>::type
        //decltype( ::cereal::serialize( std::declval<A&>(), std::declval<T&>(), 0 ) )
        >::type
        >: std::true_type {};

    template<typename T, class A>
      struct has_non_member_serialize< T, A,
      typename Void<
        typename std::enable_if<
          std::is_same<decltype(serialize( std::declval<A&>(), std::declval<T&>(), 0 )), BogusType>::value, void>::type
        //decltype( ::cereal::serialize( std::declval<A&>(), std::declval<T&>(), 0 ) )
        >::type
        >: std::false_type {};

    // ######################################################################
    template<typename T, class A, typename Sfinae = void>
      struct has_member_load: std::false_type {};

    template<typename T, class A>
      struct has_member_load< T, A,
      typename Void<
        decltype( std::declval<T&>().load( std::declval<A&>(), 0 ) )
        >::type
        >: std::true_type {};
    
    // ######################################################################
    template<typename T, class A, typename Sfinae = void>
      struct has_non_member_load: std::false_type {};

    template<typename T, class A>
      struct has_non_member_load< T, A,
      typename Void<
        decltype( ::cereal::load( std::declval<A&>(), std::declval<T&>(), 0 ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    template<typename T, class A, typename Sfinae = void>
      struct has_member_save: std::false_type {};

    template<typename T, class A>
      struct has_member_save< T, A,
      typename Void<
        decltype( std::declval<T&>().save( std::declval<A&>(), 0 ) )
        >::type
        >: std::true_type {};
    
    // ######################################################################
    template<typename T, class A, typename Sfinae = void>
      struct has_non_member_save: std::false_type {};

    template<typename T, class A>
      struct has_non_member_save< T, A,
      typename Void<
        decltype( ::cereal::save( std::declval<A&>(), std::declval<T&>(), 0 ) )
        >::type
        >: std::true_type {};

    // ######################################################################
    template <class T, class A>
      constexpr bool has_member_split()
      {
        return has_member_load<T, A>() && has_member_save<T, A>();
      }

    // ######################################################################
    template <class T, class A>
      constexpr bool has_non_member_split()
      {
        return has_non_member_load<T, A>() && has_non_member_save<T, A>();
      }

    // ######################################################################
    template <class T, class A>
      constexpr bool is_serializable()
      {
        return has_member_split<T, A>() ^ has_member_serialize<T, A>() ^ has_non_member_split<T, A>() ^ has_non_member_serialize<T,A>();
      }
  }
}
