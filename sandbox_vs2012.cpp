//#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>

#include <iostream>
#include <type_traits>

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

int main()
{
  std::cout << std::boolalpha;
  //std::cout << cereal::traits::has_member_serialize<Test, Archive>::value << std::endl;
  
  // Test Load and Allocate internal/external
  std::cout << "\tload_and_allocate" << std::endl;
  std::cout << cereal::traits::has_member_load_and_allocate<Test, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load_and_allocate<Test, Archive>::value << std::endl;

  // serialize
  std::cout << "\tserialize" << std::endl;
  std::cout << cereal::traits::has_member_serialize<Test, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_serialize<Test, Archive>::value << std::endl;

  // load
  std::cout << "\tload" << std::endl;
  std::cout << cereal::traits::has_member_load<Test, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load<Test, Archive>::value << std::endl;

  // save
  std::cout << "\tsave" << std::endl;
  std::cout << cereal::traits::has_member_save<Test, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_save<Test, Archive>::value << std::endl;

  // member split
  std::cout << "\tmember split" << std::endl;
  std::cout << cereal::traits::has_member_split<Test, Archive, Archive>::value << std::endl;

  // serialiable
  std::cout << "\toutput serializable" << std::endl;
  std::cout << cereal::traits::is_output_serializable<Test, Archive>::value << std::endl;
  
  return 0;
}