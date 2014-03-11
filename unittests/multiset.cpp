#include "common.hpp"

template <class IArchive, class OArchive>
void test_multiset()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::multiset<int> o_podmultiset;
    for(int j=0; j<100; ++j)
    {
      int value = random_value<int>(gen);
      o_podmultiset.insert(value);
      o_podmultiset.insert(value);
    }

    std::multiset<StructInternalSerialize> o_isermultiset;
    for(int j=0; j<100; ++j)
    {
      StructInternalSerialize value = { random_value<int>(gen), random_value<int>(gen) };
      o_isermultiset.insert(value);
      o_isermultiset.insert(value);
    }

    std::multiset<StructInternalSplit> o_isplmultiset;
    for(int j=0; j<100; ++j)
    {
      StructInternalSplit value = { random_value<int>(gen), random_value<int>(gen) };
      o_isplmultiset.insert(value);
      o_isplmultiset.insert(value);
    }

    std::multiset<StructExternalSerialize> o_esermultiset;
    for(int j=0; j<100; ++j)
    {
      StructExternalSerialize value = { random_value<int>(gen), random_value<int>(gen) };
      o_esermultiset.insert(value);
      o_esermultiset.insert(value);
    }

    std::multiset<StructExternalSplit> o_esplmultiset;
    for(int j=0; j<100; ++j)
    {
      StructExternalSplit value = { random_value<int>(gen), random_value<int>(gen) };
      o_esplmultiset.insert(value);
      o_esplmultiset.insert(value);
    }

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podmultiset);
      oar(o_isermultiset);
      oar(o_isplmultiset);
      oar(o_esermultiset);
      oar(o_esplmultiset);
    }

    std::multiset<int> i_podmultiset;
    std::multiset<StructInternalSerialize> i_isermultiset;
    std::multiset<StructInternalSplit>     i_isplmultiset;
    std::multiset<StructExternalSerialize> i_esermultiset;
    std::multiset<StructExternalSplit>     i_esplmultiset;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podmultiset);
      iar(i_isermultiset);
      iar(i_isplmultiset);
      iar(i_esermultiset);
      iar(i_esplmultiset);
    }

    for(auto const & p : i_podmultiset)
    {
      BOOST_CHECK_EQUAL(o_podmultiset.count(p), i_podmultiset.count(p));
    }

    for(auto const & p : i_isermultiset)
    {
      BOOST_CHECK_EQUAL(o_isermultiset.count(p), i_isermultiset.count(p));
    }

    for(auto const & p : i_isplmultiset)
    {
      BOOST_CHECK_EQUAL(o_isplmultiset.count(p), i_isplmultiset.count(p));
    }

    for(auto const & p : i_esermultiset)
    {
      BOOST_CHECK_EQUAL(o_esermultiset.count(p), i_esermultiset.count(p));
    }

    for(auto const & p : i_esplmultiset)
    {
      BOOST_CHECK_EQUAL(o_esplmultiset.count(p), i_esplmultiset.count(p));
    }
  }
}

BOOST_AUTO_TEST_CASE( binary_multiset )
{
  test_multiset<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_multiset )
{
  test_multiset<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_multiset )
{
  test_multiset<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_multiset )
{
  test_multiset<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


