#include "cereal.hpp"
#include <cxxabi.h>
#include <sstream>
#include <fstream>

// ###################################
struct Test1
{
  int a;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar & a;
  }
};

// ###################################
struct Test2
{
  int a;

  template<class Archive>
  void save(Archive & ar) const
  {
    ar & a;
  }

  template<class Archive>
  void load(Archive & ar)
  {
    ar & a;
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
  ar & t.a;
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
    ar & t.a;
  }

  template<class Archive>
  void load(Archive & ar, Test4 & t)
  {
    ar & t.a;
  }
}

// ######################################################################
int main()
{
  std::ostringstream os;
  cereal::BinaryOutputArchive archive(os);

  Test1 t1;
  Test2 t2;
  Test3 t3;
  test4::Test4 t4;

  archive & t1;
  archive & t2;
  archive & t3;
  archive & t4;

  return 0;
}
