#include "cereal.hpp"
#include <cxxabi.h>

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

  template<class Archive>
void serialize(Archive & ar, Test3 & t, unsigned int version)
{
  //ar & t.a;
  //ar & t.b;
}

// ###################################
struct Test4
{
  int a;
  std::string b;

};

  template<class Archive>
void save(Archive & ar, Test4 & t, unsigned int version)
{
  ar & t.a & t.b;
}

  template<class Archive>
void load(Archive & ar, Test4 & t, unsigned int version)
{
  ar & t.a & t.b;
}

// ######################################################################
int main()
{
  cereal::BinaryOutputArchive archive;

  Test1 t1;
  Test2 t2;
  Test3 t3;
  Test4 t4;

  archive & t1;
  archive & t2;
  archive & t3;
  archive & t4;

  return 0;
}
