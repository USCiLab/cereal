#pragma once

#include <iostream>
#include <type_traits>

namespace cereal
{
  namespace detail
  {
    template<typename> struct Void { typedef void type; };

    template<typename T, class A, typename Sfinae = void>
      struct has_serialize: std::false_type {};

    template<typename T, class A>
      struct has_serialize< T, A,
      typename Void<
        decltype( std::declval<T&>().serialize( std::declval<A&>(), 0 ) )
        >::type
        >: std::true_type {};

    template<typename T, class A, typename Sfinae = void>
      struct has_load: std::false_type {};

    template<typename T, class A>
      struct has_load< T, A,
      typename Void<
        decltype( std::declval<T&>().load( std::declval<A&>(), 0 ) )
        >::type
        >: std::true_type {};

    template<typename T, class A, typename Sfinae = void>
      struct has_save: std::false_type {};

    template<typename T, class A>
      struct has_save< T, A,
      typename Void<
        decltype( std::declval<T&>().save( std::declval<A&>(), 0 ) )
        >::type
        >: std::true_type {};

    template <class T, class A>
      constexpr bool has_split()
      {
        return has_load<T, A>() && has_save<T, A>();
      }

    template <class T, class A>
      constexpr bool is_serializable()
      {
        return has_split<T, A>() ^ has_serialize<T, A>();
      }
  }

    class BinaryOutputArchive
    {
      public:

        BinaryOutputArchive() {}

        BinaryOutputArchive(std::ostream & stream)
        {
        }

        template<class T>
          typename std::enable_if<detail::is_serializable<T, BinaryOutputArchive>() && detail::has_serialize<T, BinaryOutputArchive>(),
                   BinaryOutputArchive &>::type
          operator & (T const & t)
        { 
          std::cout << "Saving non-split member" << std::endl;
          //t.serialize(*this, detail::version<T>::value)
          return *this;
        }

        template<class T>
          typename std::enable_if<detail::is_serializable<T, BinaryOutputArchive>() && detail::has_split<T, BinaryOutputArchive>(),
                   BinaryOutputArchive &>::type
          operator & (T const & t)
        { 
          std::cout << "Saving split member" << std::endl;
          //t.save(*this, detail::version<T>::value);
          return *this;
        }

        template<class T>
          typename std::enable_if<!detail::is_serializable<T, BinaryOutputArchive>(), BinaryOutputArchive &>::type
          operator & (T const & t)
        { 
          static_assert(detail::is_serializable<T, BinaryOutputArchive>(), "Trying to serialize an unserializable type.\n\n"
              "Types must either have a serialize function, or separate save/load functions (but not both).\n"
              "Serialize functions generally have the following signature:\n\n"
              "template<class Archive>\n"
              "  void serialize(int & ar, unsigned int version)\n"
              "  {\n"
              "    ar & member1 & member2 & member3;\n"
              "  }\n\n" );
          return *this;
        }


    };

}
