#include "common.hpp"

template <class IArchive, class OArchive>
void test_array()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::array<int, 100> o_podarray;
    for(auto & elem : o_podarray)
      elem = random_value<int>(gen);

    std::array<StructInternalSerialize, 100> o_iserarray;
    for(auto & elem : o_iserarray)
      elem = StructInternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::array<StructInternalSplit, 100> o_isplarray;
    for(auto & elem : o_isplarray)
      elem = StructInternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::array<StructExternalSerialize, 100> o_eserarray;
    for(auto & elem : o_eserarray)
      elem = StructExternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::array<StructExternalSplit, 100> o_esplarray;
    for(auto & elem : o_esplarray)
      elem = StructExternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podarray);
      oar(o_iserarray);
      oar(o_isplarray);
      oar(o_eserarray);
      oar(o_esplarray);
    }

    std::array<int, 100> i_podarray;
    std::array<StructInternalSerialize, 100> i_iserarray;
    std::array<StructInternalSplit, 100>     i_isplarray;
    std::array<StructExternalSerialize, 100> i_eserarray;
    std::array<StructExternalSplit, 100>     i_esplarray;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podarray);
      iar(i_iserarray);
      iar(i_isplarray);
      iar(i_eserarray);
      iar(i_esplarray);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podarray.begin(),    i_podarray.end(),    o_podarray.begin(),  o_podarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserarray.begin(),   i_iserarray.end(),   o_iserarray.begin(), o_iserarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplarray.begin(),   i_isplarray.end(),   o_isplarray.begin(), o_isplarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserarray.begin(),   i_eserarray.end(),   o_eserarray.begin(), o_eserarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplarray.begin(),   i_esplarray.end(),   o_esplarray.begin(), o_esplarray.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_array )
{
  test_array<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_array )
{
  test_array<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_array )
{
  test_array<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_array )
{
  test_array<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

