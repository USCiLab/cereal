#include "test_class.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_xml)
{
  const char* const xml =
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
    "<cereal>"
      "<value0>"
        "<polymorphic_id>2147483649</polymorphic_id>"
        "<polymorphic_name>TestType</polymorphic_name>"
        "<ptr_wrapper>"
        "<id>2147483649</id>"
        "<data>"
          "<value0>123</value0>"
        "</data>"
        "</ptr_wrapper>"
    "</value0>"
  "</cereal>";
  
  std::stringstream stream(xml);

  stream.seekg(0);

  {
    cereal::XMLInputArchive in(stream);
    TestTypeBasePtr ptr;
    in(ptr);

    BOOST_REQUIRE((bool)ptr);
    auto casted = std::dynamic_pointer_cast<TestType>(ptr);
    BOOST_REQUIRE((bool)casted);
    BOOST_CHECK_EQUAL("123", casted->str);
  }
}
