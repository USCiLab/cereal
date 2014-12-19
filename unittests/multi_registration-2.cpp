#include "multi_registration-common.hpp"
#include "multi_registration-2-archive.hpp"
#include <boost/test/unit_test.hpp>

namespace
{

template <class IArchive, class OArchive>
void test()
{
  std::stringstream stream;
  TestTypeBasePtr org = std::make_shared<TestType>("123");
  TestType expected("123, specially saved, specially loaded");
  
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

BOOST_AUTO_TEST_CASE(xml2)
{
  test<TestArchive<cereal::XMLInputArchiveT>, TestArchive<cereal::XMLOutputArchiveT>>();
}

BOOST_AUTO_TEST_CASE(binary2)
{
  test<TestArchive<cereal::BinaryInputArchiveT>, TestArchive<cereal::BinaryOutputArchiveT>>();
}

BOOST_AUTO_TEST_CASE(portable_binary2)
{
  test<TestArchive<cereal::PortableBinaryInputArchiveT>, TestArchive<cereal::PortableBinaryOutputArchiveT>>();
}

BOOST_AUTO_TEST_CASE(json2)
{
  test<TestArchive<cereal::JSONInputArchiveT>, TestArchive<cereal::JSONOutputArchiveT>>();
}
