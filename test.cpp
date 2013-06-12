#include "cereal.hpp"

// ###################################
struct Test1
{
  int a;
  std::string b;

  template<class Archive>
    void serialize(Archive & ar, unsigned int version)
    {
      ar & a & b;
    }
};

// ###################################
struct Test2
{
  int a;
  std::string b;

  template<class Archive>
    void save(Archive & ar, unsigned int version)
    {
      ar & a & b;
    }

  template<class Archive>
    void load(Archive & ar, unsigned int version)
    {
      ar & a & b;
    }
};

// ###################################
struct Test3
{
  int a;
  std::string b;

};

#if 1
//namespace cereal
//{
  template<class Archive>
    void serialize(Archive & ar, Test3 & t, unsigned int version)
    {
      //ar & t.a;
      //ar & t.b;
    }
//}
#endif

// ###################################
struct Test4
{
  int a;
  std::string b;

  template<class Archive>
    void serialize(Archive & ar, unsigned int version)
    {
      ar & a & b;
    }

  template<class Archive>
    void save(Archive & ar, unsigned int version)
    {
      ar & a & b;
    }

  template<class Archive>
    void load(Archive & ar, unsigned int version)
    {
      ar & a & b;
    }
};

//// ######################################################################
//template<typename> struct Void { typedef void type; };
//
//template<typename T, class A, typename Sfinae = void>
//struct has_non_member_serialize2: std::false_type {};
//
//template<typename T, class A>
//struct has_non_member_serialize2< T, A,
//  typename Void<
//decltype( cereal::serialize( std::declval<A&>(), std::declval<T&>(), 0 ) )
//  >::type
//  >: std::true_type {};

// ######################################################################
int main()
{
  cereal::BinaryOutputArchive archive;

  Test1 t1;
  Test2 t2;
  Test3 t3;
  Test4 t4;

  //archive & t1;
  //archive & t2;
  //archive & t3;
  //archive & t4;

  //cereal::serialize(archive, t3, 0);


  std::cout << cereal::traits::has_non_member_serialize<Test2, cereal::BinaryOutputArchive>() << std::endl;
  //std::cout << has_non_member_serialize2<Test3, cereal::BinaryOutputArchive>() << std::endl;

  return 0;
}
