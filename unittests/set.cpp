#include "common.hpp"

template <class IArchive, class OArchive>
void test_set()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::set<int> o_podset;
    for(int j=0; j<100; ++j)
      o_podset.insert(random_value<int>(gen));

    std::set<StructInternalSerialize> o_iserset;
    for(int j=0; j<100; ++j)
      o_iserset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructInternalSplit> o_isplset;
    for(int j=0; j<100; ++j)
      o_isplset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructExternalSerialize> o_eserset;
    for(int j=0; j<100; ++j)
      o_eserset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructExternalSplit> o_esplset;
    for(int j=0; j<100; ++j)
      o_esplset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podset);
      oar(o_iserset);
      oar(o_isplset);
      oar(o_eserset);
      oar(o_esplset);
    }

    std::set<int> i_podset;
    std::set<StructInternalSerialize>   i_iserset;
    std::set<StructInternalSplit>        i_isplset;
    std::set<StructExternalSerialize> i_eserset;
    std::set<StructExternalSplit>       i_esplset;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podset);
      iar(i_iserset);
      iar(i_isplset);
      iar(i_eserset);
      iar(i_esplset);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podset.begin(),    i_podset.end(),    o_podset.begin(),  o_podset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserset.begin(),   i_iserset.end(),   o_iserset.begin(), o_iserset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplset.begin(),   i_isplset.end(),   o_isplset.begin(), o_isplset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserset.begin(),   i_eserset.end(),   o_eserset.begin(), o_eserset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplset.begin(),   i_esplset.end(),   o_esplset.begin(), o_esplset.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_set )
{
  test_set<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_set )
{
  test_set<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_set )
{
  test_set<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_set )
{
  test_set<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

