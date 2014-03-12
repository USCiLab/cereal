#include "common.hpp"

template <class IArchive, class OArchive>
void test_list()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::list<int> o_podlist(100);
    for(auto & elem : o_podlist)
      elem = random_value<int>(gen);

    std::list<StructInternalSerialize> o_iserlist(100);
    for(auto & elem : o_iserlist)
      elem = StructInternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::list<StructInternalSplit> o_ispllist(100);
    for(auto & elem : o_ispllist)
      elem = StructInternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::list<StructExternalSerialize> o_eserlist(100);
    for(auto & elem : o_eserlist)
      elem = StructExternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::list<StructExternalSplit> o_espllist(100);
    for(auto & elem : o_espllist)
      elem = StructExternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podlist);
      oar(o_iserlist);
      oar(o_ispllist);
      oar(o_eserlist);
      oar(o_espllist);
    }

    std::list<int> i_podlist;
    std::list<StructInternalSerialize> i_iserlist;
    std::list<StructInternalSplit>     i_ispllist;
    std::list<StructExternalSerialize> i_eserlist;
    std::list<StructExternalSplit>     i_espllist;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podlist);
      iar(i_iserlist);
      iar(i_ispllist);
      iar(i_eserlist);
      iar(i_espllist);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podlist.begin(),    i_podlist.end(),    o_podlist.begin(),  o_podlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserlist.begin(),   i_iserlist.end(),   o_iserlist.begin(), o_iserlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_ispllist.begin(),   i_ispllist.end(),   o_ispllist.begin(), o_ispllist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserlist.begin(),   i_eserlist.end(),   o_eserlist.begin(), o_eserlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_espllist.begin(),   i_espllist.end(),   o_espllist.begin(), o_espllist.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_list )
{
  test_list<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_list )
{
  test_list<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_list )
{
  test_list<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_list )
{
  test_list<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

