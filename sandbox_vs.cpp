#define BOOST_ALL_NO_LIB

#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/cereal.hpp>

#include <cereal/types/array.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>

#include <cereal/details/util.hpp>

#include <cereal/details/polymorphic_impl.hpp>
#include <cereal/types/polymorphic.hpp>

#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include <cereal/archives/binary.hpp>
//#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

#include <iostream>
#include <type_traits>
#include <functional>

struct Archive {};

struct Test
{
  template <class Archive>
  void serialzize( Archive & ar )
  {
    std::cout << "hey there" << std::endl;
  }

  template <class Archive>
  void save( Archive & ar ) const
  {
    std::cout << "saved by the bell" << std::endl;
  }

  template <class Archive>
  void load( Archive & ar )
  {
    std::cout << "locked and loaded" << std::endl;
  }

  template <class Archive>
  static Test * load_and_allocate( Archive & ar )
  {
    return new Test();
  }
};

template <class Archive>
void serialize( Archive & ar, Test & t )
{ }

template <class Archive>
void load( Archive & ar, Test & t )
{ }

template <class Archive>
void save( Archive & ar, Test const & t )
{ }

namespace cereal
{
  template <>
  struct LoadAndAllocate<Test>
  {
    template <class Archive>
    static Test * load_and_allocate( Archive & ar )
    {
      return new Test();
    }
  };
}

struct A
{
  virtual void foo() = 0;
};

struct B : A
{
  void foo() {}

  template <class Archive>
  void serialize( Archive & ar )
  {
    std::cout << "i'm in your b" << std::endl;
  }
};

struct C
{
  char a;
};

//CEREAL_REGISTER_TYPE(B);

template <class T, class A>
static auto test(int) -> decltype( cereal::access::member_serialize( std::declval<A&>(), std::declval<T&>() ), std::true_type())
{ return {}; }

template <class T, class A>
static auto test(...) -> std::false_type
{ return {}; }

int main()
{
  typedef Test T;
  std::cout << std::boolalpha;

  // Test Load and Allocate internal/external
  std::cout << "\tload_and_allocate" << std::endl;
  std::cout << cereal::traits::has_member_load_and_allocate<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load_and_allocate<T, Archive>::value << std::endl;

  // serialize
  std::cout << "\tserialize" << std::endl;
  std::cout << cereal::traits::has_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_serialize<T, Archive>::value << std::endl;
  std::cout << test<T, Archive>(0) << std::endl;

  // load
  std::cout << "\tload" << std::endl;
  std::cout << cereal::traits::has_member_load<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load<T, Archive>::value << std::endl;

  // save
  std::cout << "\tsave" << std::endl;
  std::cout << cereal::traits::has_member_save<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_save<T, Archive>::value << std::endl;

  // splittable
  std::cout << "\t splittable" << std::endl;
  std::cout << cereal::traits::has_member_split<T, Archive, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_split<T, Archive, Archive>::value << std::endl;

  // serialiable
  std::cout << "\toutput serializable" << std::endl;
  std::cout << cereal::traits::is_output_serializable<T, Archive>::value << std::endl;
  std::cout << cereal::traits::is_input_serializable<T, Archive>::value << std::endl;

  // specialized
  std::cout << "\tspecialized" << std::endl;
  std::cout << cereal::traits::detail::is_specialized_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_member_load_save<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_non_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_non_member_load_save<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_error<T, Archive>::value << std::endl;
  std::cout << cereal::traits::is_specialized<T, Archive>::value << std::endl;

  // array size
  std::cout << typeid(A).name() << std::endl;
  std::cout << typeid(cereal::traits::has_load_and_allocate<int, bool>).name() << std::endl;

  //Archive a;
  //T t;

  //cereal::access::member_save( a, t );
  //cereal::access::member_load( a, t );
  //cereal::access::member_serialize( a, t );

  //std::stringstream ss;
  //{
  //  cereal::JSONOutputArchive ar( ss );
  //  ar( 5 );
  //  ar( cereal::make_nvp("hello", 2.4f ) );
  //  std::string s = "hey yo";
  //  ar( CEREAL_NVP( s ) );
  //  int darp [] = { 1, 2, 3 };
  //  ar.saveBinaryValue( darp, sizeof(int) * 3, "darp" );
  //  std::unique_ptr<A> ptr( new B() );
  //  ar( CEREAL_NVP( ptr ) );
  //}
  //{
  //  cereal::JSONInputArchive ar( ss );
  //  int x;
  //  ar( x );
  //  assert( x == 5 );
  //  float f;
  //  ar( f );
  //  assert( f == 2.4f );
  //  std::string s;
  //  ar( s );
  //  assert( s == "hey yo" );
  //  int darp[3];
  //  ar.loadBinaryValue( darp, sizeof(int) * 3 );
  //  assert( darp[0] == 1 );
  //  assert( darp[1] == 2 );
  //  assert( darp[2] == 3 );
  //  std::unique_ptr<A> ptr;
  //  std::cout << "----------" << std::endl;
  //  std::cout << std::is_default_constructible<A>::value << std::endl;
  //  std::cout << cereal::traits::has_load_and_allocate<A, cereal::JSONInputArchive>::value << std::endl;
  //  ar( ptr );
  //}

  return 0;
}
