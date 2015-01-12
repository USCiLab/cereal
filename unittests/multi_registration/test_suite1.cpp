#include "common.hpp"
#include <boost/test/unit_test.hpp>

namespace
{

template <class IArchive, class OArchive>
void test()
{
  std::stringstream stream;
  TestTypeBasePtr org = std::make_shared<TestType>("123");
  TestType expected("123, normally saved, normally loaded");
  
  {
    OArchive out(stream);
    out(org);
  }

  stream.seekg(0);

  {
    IArchive in(stream);
    TestTypeBasePtr ptr;
    in(ptr);

    BOOST_REQUIRE((bool)ptr);
    auto casted = std::dynamic_pointer_cast<TestType>(ptr);
    BOOST_REQUIRE((bool)casted);
    BOOST_CHECK_EQUAL(expected.str, casted->str);
  }
}

} // anon namespace

BOOST_AUTO_TEST_CASE(xml1)
{
  test<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE(binary1)
{
  test<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE(portable_binary1)
{
  test<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE(json1)
{
  test<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
