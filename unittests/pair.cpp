#include "common.hpp"

template <class IArchive, class OArchive>
void test_pair()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    std::pair<int, int> o_podpair = {rng(), rng()};
    std::pair<StructInternalSerialize, StructInternalSerialize> o_iserpair = {{rng(), rng()}, {rng(), rng()}};
    std::pair<StructInternalSplit, StructInternalSplit> o_isplpair = {{rng(), rng()}, {rng(), rng()}};
    std::pair<StructExternalSerialize, StructExternalSerialize> o_eserpair = {{rng(), rng()}, {rng(), rng()}};
    std::pair<StructExternalSplit, StructExternalSplit> o_esplpair = {{rng(), rng()}, {rng(), rng()}};

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podpair);
      oar(o_iserpair);
      oar(o_isplpair);
      oar(o_eserpair);
      oar(o_esplpair);
    }

    std::pair<int, int> i_podpair;
    std::pair<StructInternalSerialize, StructInternalSerialize> i_iserpair;
    std::pair<StructInternalSplit, StructInternalSplit> i_isplpair;
    std::pair<StructExternalSerialize, StructExternalSerialize> i_eserpair;
    std::pair<StructExternalSplit, StructExternalSplit> i_esplpair;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podpair);
      iar(i_iserpair);
      iar(i_isplpair);
      iar(i_eserpair);
      iar(i_esplpair);
    }

    BOOST_CHECK_EQUAL( i_podpair.first, o_podpair.first );
    BOOST_CHECK_EQUAL( i_podpair.second, o_podpair.second );

    BOOST_CHECK_EQUAL( i_iserpair.first, o_iserpair.first );
    BOOST_CHECK_EQUAL( i_iserpair.second, o_iserpair.second );

    BOOST_CHECK_EQUAL( i_isplpair.first, o_isplpair.first );
    BOOST_CHECK_EQUAL( i_isplpair.second, o_isplpair.second );

    BOOST_CHECK_EQUAL( i_eserpair.first, o_eserpair.first );
    BOOST_CHECK_EQUAL( i_eserpair.second, o_eserpair.second );

    BOOST_CHECK_EQUAL( i_esplpair.first, o_esplpair.first );
    BOOST_CHECK_EQUAL( i_esplpair.second, o_esplpair.second );
  }
}

BOOST_AUTO_TEST_CASE( binary_pair )
{
  test_pair<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_pair )
{
  test_pair<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_pair )
{
  test_pair<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}
BOOST_AUTO_TEST_CASE( json_pair )
{
  test_pair<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

