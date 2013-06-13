#include "cereal.hpp"
#include "binary_archive.hpp"
#include "json_archive.hpp"
#include <cxxabi.h>
#include <sstream>
#include <fstream>
#include <cassert>

// ###################################
struct Test1
{
  int a;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar & CEREAL_NVP(a);
  }
};

// ###################################
struct Test2
{
  int a;

  template<class Archive>
  void save(Archive & ar) const
  {
    ar & CEREAL_NVP(a);
  }

  template<class Archive>
  void load(Archive & ar)
  {
    ar & CEREAL_NVP(a);
  }
};

// ###################################
struct Test3
{
  int a;
};

template<class Archive>
void serialize(Archive & ar, Test3 & t)
{
  ar & CEREAL_NVP(t.a);
}

namespace test4
{
  // ###################################
  struct Test4
  {
    int a;
  };

  template<class Archive>
  void save(Archive & ar, Test4 const & t)
  {
    ar & CEREAL_NVP(t.a);
  }

  template<class Archive>
  void load(Archive & ar, Test4 & t)
  {
    ar & CEREAL_NVP(t.a);
  }
}

struct Everything
{
  int x;
  int y;
  Test1 t1;
  Test2 t2;
  Test3 t3;
  test4::Test4 t4;
  std::string s;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar & CEREAL_NVP(x);
    ar & CEREAL_NVP(y);
    ar & CEREAL_NVP(t1);
    ar & CEREAL_NVP(t2);
    ar & CEREAL_NVP(t3);
    ar & CEREAL_NVP(t4);
    ar & CEREAL_NVP(s);
  }

  bool operator==(Everything const & o)
  {
    return
      x == o.x &&
      y == o.y &&
      t1.a == o.t1.a &&
      t2.a == o.t2.a &&
      t3.a == o.t3.a &&
      t4.a == o.t4.a &&
      s == o.s;
  }

};

// ######################################################################
int main()
{
  Everything e_out;
  e_out.x = 99;
  e_out.y = 100;
  e_out.t1 = {1};
  e_out.t2 = {2};
  e_out.t3 = {3};
  e_out.t4 = {4};
  e_out.s = "Hello, World!";

  {
    std::ofstream os("out.txt");
    cereal::BinaryOutputArchive archive(os);
    archive & CEREAL_NVP(e_out);
  }

  Everything e_in;

  {
    std::ifstream is("out.txt");
    cereal::BinaryInputArchive archive(is);
    archive & CEREAL_NVP(e_in);
  }

  assert(e_in == e_out);

  cereal::JSONOutputArchive json(std::cout);

  std::string hello = "Hello, World!";
  json & CEREAL_NVP(hello);
  //json & CEREAL_NVP(e_out); <<< Need to figure out how to recurse!

  return 0;
}
