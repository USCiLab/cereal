#include "common.hpp"

template <class IArchive, class OArchive>
void test_chrono()
{
  std::chrono::steady_clock::now();
  //for(int ii=0; ii<100; ++ii)
  //{
  //  auto o_timePoint1 = std::chrono::system_clock::now();
  //  #ifndef CEREAL_OLDER_GCC
  //  auto o_timePoint2 = std::chrono::steady_clock::now();
  //  #endif // CEREAL_OLDER_GCC
  //  auto o_timePoint3 = std::chrono::high_resolution_clock::now();

  //  auto o_duration1 = std::chrono::system_clock::now() - o_timePoint1;
  //  #ifndef CEREAL_OLDER_GCC
  //  auto o_duration2 = std::chrono::steady_clock::now() - o_timePoint2;
  //  #endif // CEREAL_OLDER_GCC
  //  auto o_duration3 = std::chrono::high_resolution_clock::now() - o_timePoint3;

  //  std::ostringstream os;
  //  {
  //    OArchive oar(os);

  //    oar(o_timePoint1);
  //    #ifndef CEREAL_OLDER_GCC
  //    oar(o_timePoint2);
  //    #endif // CEREAL_OLDER_GCC
  //    oar(o_timePoint3);
  //    oar(o_duration1);
  //    #ifndef CEREAL_OLDER_GCC
  //    oar(o_duration2);
  //    #endif // CEREAL_OLDER_GCC
  //    oar(o_duration3);
  //  }

  //  decltype(o_timePoint1) i_timePoint1;
  //  #ifndef CEREAL_OLDER_GCC
  //  decltype(o_timePoint2) i_timePoint2;
  //  #endif // CEREAL_OLDER_GCC
  //  decltype(o_timePoint3) i_timePoint3;
  //  decltype(o_duration1) i_duration1;
  //  #ifndef CEREAL_OLDER_GCC
  //  decltype(o_duration2) i_duration2;
  //  #endif // CEREAL_OLDER_GCC
  //  decltype(o_duration3) i_duration3;

  //  std::istringstream is(os.str());
  //  {
  //    IArchive iar(is);

  //    iar(i_timePoint1);
  //    #ifndef CEREAL_OLDER_GCC
  //    iar(i_timePoint2);
  //    #endif // CEREAL_OLDER_GCC
  //    iar(i_timePoint3);
  //    iar(i_duration1);
  //    #ifndef CEREAL_OLDER_GCC
  //    iar(i_duration2);
  //    #endif // CEREAL_OLDER_GCC
  //    iar(i_duration3);
  //  }

  //  BOOST_CHECK( o_timePoint1 == i_timePoint1 );
  //  #ifndef CEREAL_OLDER_GCC
  //  BOOST_CHECK( o_timePoint2 == i_timePoint2 );
  //  #endif // CEREAL_OLDER_GCC
  //  BOOST_CHECK( o_timePoint3 == i_timePoint3 );
  //  BOOST_CHECK( o_duration1 == i_duration1 );
  //  #ifndef CEREAL_OLDER_GCC
  //  BOOST_CHECK( o_duration2 == i_duration2 );
  //  #endif // CEREAL_OLDER_GCC
  //  BOOST_CHECK( o_duration3 == i_duration3 );
  //}
}

BOOST_AUTO_TEST_CASE( binary_chrono )
{
  test_chrono<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_chrono )
{
  test_chrono<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_chrono )
{
  test_chrono<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_chrono )
{
  test_chrono<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


