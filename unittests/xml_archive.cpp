#include "common.hpp"
#include <boost/test/unit_test.hpp>



// Tests the default behavior to swap bytes to current machine's endianness
BOOST_AUTO_TEST_CASE( xml_rootnodename )
{
  std::ostringstream os;
  {
    cereal::XMLOutputArchive oar(os, cereal::XMLOutputArchive::Options(std::numeric_limits<double>::max_digits10, true, false, "xmlroot"));
    int32_t i_int32 = 42;
    oar(CEREAL_NVP(i_int32));
  }
  
  int32_t i_int32 = 0;

  std::istringstream is(os.str());
  {
    cereal::XMLInputArchive iar(is, "xmlroot");
    iar(i_int32);
  }
  BOOST_CHECK_EQUAL(i_int32, 42);
}