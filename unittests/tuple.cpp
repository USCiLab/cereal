#include "common.hpp"

template <class IArchive, class OArchive>
void test_tuple()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    auto o_podtuple = std::make_tuple( rng(), rng(), rng(), rng() );
    auto o_isertuple = std::make_tuple( StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ) );
    auto o_ispltuple = std::make_tuple( StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ) );
    auto o_esertuple = std::make_tuple( StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ) );
    auto o_espltuple = std::make_tuple( StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podtuple);
      oar(o_isertuple);
      oar(o_ispltuple);
      oar(o_esertuple);
      oar(o_espltuple);
    }

    decltype( o_podtuple  ) i_podtuple;
    decltype( o_isertuple ) i_isertuple;
    decltype( o_ispltuple ) i_ispltuple;
    decltype( o_esertuple ) i_esertuple;
    decltype( o_espltuple ) i_espltuple;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podtuple);
      iar(i_isertuple);
      iar(i_ispltuple);
      iar(i_esertuple);
      iar(i_espltuple);
    }

    BOOST_CHECK_EQUAL( i_podtuple == o_podtuple, true );
    BOOST_CHECK_EQUAL( i_isertuple == o_isertuple, true );
    BOOST_CHECK_EQUAL( i_ispltuple == o_ispltuple, true );
    BOOST_CHECK_EQUAL( i_esertuple == o_esertuple, true );
    BOOST_CHECK_EQUAL( i_espltuple == o_espltuple, true );
  }
}

BOOST_AUTO_TEST_CASE( binary_tuple )
{
  test_tuple<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_tuple )
{
  test_tuple<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_tuple )
{
  test_tuple<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_tuple )
{
  test_tuple<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

