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

  template<class Archive>
    void serialize(int & ar, unsigned int version)
    {
      ar & a;
    }
};

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
  //archive & t3;
  //archive & t4;




  return 0;
}
