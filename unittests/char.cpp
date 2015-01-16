#include "common.hpp"
#include <boost/test/unit_test.hpp>

template <class IArchive, class OArchive>
void test_char()
{
  const std::string test_data = "a "; // The space is important to test
  
  for (const auto orgCh: test_data)
  {
    std::ostringstream os;
    {
      OArchive oar(os);

      oar(orgCh);
    }

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      char ch;

      iar(ch);
      BOOST_CHECK_EQUAL(orgCh, ch);
    }
  }
}

BOOST_AUTO_TEST_CASE( binary_array )
{
  test_char<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_array )
{
  test_char<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_array )
{
  test_char<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_array )
{
  test_char<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
