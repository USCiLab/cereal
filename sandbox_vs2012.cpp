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
//#include <cereal/archives/json.hpp>

#include <iostream>
#include <type_traits>
#include <functional>

struct Archive {};

struct Test
{
  template <class Archive>
  void serialize( Archive & ar )
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
  { }
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
    { }
  };
}

struct A
{
  virtual void foo() = 0;
};

struct B : A
{
  void foo() {}
};

struct C
{
  char a;
};

CEREAL_REGISTER_TYPE(B);

int main()
{
  std::cout << std::boolalpha;

  typedef Test T;
  
  // Test Load and Allocate internal/external
  std::cout << "\tload_and_allocate" << std::endl;
  std::cout << cereal::traits::has_member_load_and_allocate<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load_and_allocate<T, Archive>::value << std::endl;

  // serialize
  std::cout << "\tserialize" << std::endl;
  std::cout << cereal::traits::has_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_serialize<T, Archive>::value << std::endl;

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
  int x[5][3][2];
  std::cout << "\tarray size" << std::endl;
  std::cout << cereal::traits::sizeof_array<decltype(x)>::value << std::endl;

  std::cout << typeid(A).name() << std::endl;
  std::cout << typeid(cereal::traits::has_load_and_allocate<int, bool>).name() << std::endl;

  
  Archive a;
  T t;
  

  cereal::access::member_save( a, t );
  cereal::access::member_load( a, t );
  cereal::access::member_serialize( a, t );

  std::stringstream ss;
  {
    cereal::XMLOutputArchive ar( ss, 20, true );
    ar( 5 );
    ar( cereal::make_nvp("hello", 2.4f ) );
    std::string s = "hey yo";
    ar( CEREAL_NVP( s ) );
  }
  {
    cereal::XMLInputArchive ar( ss );
    int x;
    ar( x );
    assert( x == 5 );
    float f;
    ar( f );
    assert( f == 2.4f );
    std::string s;
    ar( s );
    assert( s == "hey yo" );
  }
   
  return 0;
}