#include "common.hpp"

template <class IArchive, class OArchive>
void test_structs()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    StructInternalSerialize o_iser = { random_value<int>(gen), random_value<int>(gen) };
    StructInternalSplit     o_ispl = { random_value<int>(gen), random_value<int>(gen) };
    StructExternalSerialize o_eser = { random_value<int>(gen), random_value<int>(gen) };
    StructExternalSplit     o_espl = { random_value<int>(gen), random_value<int>(gen) };

    std::ostringstream os;
    {
      OArchive oar(os);
      oar( o_iser, o_ispl, o_eser, o_espl);
    }

    StructInternalSerialize i_iser;
    StructInternalSplit     i_ispl;
    StructExternalSerialize i_eser;
    StructExternalSplit     i_espl;

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      iar( i_iser, i_ispl, i_eser, i_espl);
    }

    BOOST_CHECK(i_iser == o_iser);
    BOOST_CHECK(i_ispl == o_ispl);
    BOOST_CHECK(i_eser == o_eser);
    BOOST_CHECK(i_espl == o_espl);
  }
}

BOOST_AUTO_TEST_CASE( binary_structs )
{
  test_structs<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_structs )
{
  test_structs<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_structs )
{
  test_structs<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_structs )
{
  test_structs<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
