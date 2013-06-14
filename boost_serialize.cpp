#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/config.hpp>
#include <vector>
#include <iostream>

struct myStruct
{
  myStruct() : x(0) {}
  int x;


  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & x;
  }
};


int main()
{
  boost::archive::text_oarchive oa(std::cout);

  const myStruct v;

  oa & v;

  std::cout << BOOST_NO_FUNCTION_TEMPLATE_ORDERING << std::endl;

  return 0;
}
