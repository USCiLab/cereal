#include "common.hpp"

template <class IArchive, class OArchive>
void test_multimap()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::multimap<std::string, int> o_podmultimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<std::string>(gen);
      o_podmultimap.insert({key, random_value<int>(gen)});
      o_podmultimap.insert({key, random_value<int>(gen)});
    }

    std::multimap<uint8_t, StructInternalSerialize> o_isermultimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<uint8_t>(gen);
      o_isermultimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
      o_isermultimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::multimap<int16_t, StructInternalSplit> o_isplmultimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<int16_t>(gen);
      o_isplmultimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
      o_isplmultimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::multimap<uint32_t, StructExternalSerialize> o_esermultimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<uint32_t>(gen);
      o_esermultimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
      o_esermultimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::multimap<int8_t, StructExternalSplit> o_esplmultimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<char>(gen);
      o_esplmultimap.insert({key,  { random_value<int>(gen), random_value<int>(gen) }});
      o_esplmultimap.insert({key,  { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podmultimap);
      oar(o_isermultimap);
      oar(o_isplmultimap);
      oar(o_esermultimap);
      oar(o_esplmultimap);
    }

    std::multimap<std::string, int> i_podmultimap;
    std::multimap<uint8_t, StructInternalSerialize>   i_isermultimap;
    std::multimap<int16_t, StructInternalSplit>        i_isplmultimap;
    std::multimap<uint32_t, StructExternalSerialize> i_esermultimap;
    std::multimap<int8_t, StructExternalSplit>       i_esplmultimap;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podmultimap);
      iar(i_isermultimap);
      iar(i_isplmultimap);
      iar(i_esermultimap);
      iar(i_esplmultimap);
    }

#define MULTIMAP_CHECK(InMap, OutMap) \
    for( auto & pair : OutMap ) \
    { \
      auto const count = InMap.count( pair.first ); \
      BOOST_CHECK_EQUAL( count, OutMap.count( pair.first ) ); \
      auto find = InMap.find( pair.first ); \
      bool found = false; \
      for( size_t i = 0; i < count; ++i, ++find ) \
        found |= find->second == pair.second; \
      BOOST_CHECK( found ); \
    }

    MULTIMAP_CHECK( i_podmultimap, o_podmultimap );
    MULTIMAP_CHECK( i_isermultimap, o_isermultimap );
    MULTIMAP_CHECK( i_isplmultimap, o_isplmultimap );
    MULTIMAP_CHECK( i_esermultimap, o_esermultimap );
    MULTIMAP_CHECK( i_esplmultimap, o_esplmultimap );

#undef MULTIMAP_CHECK
  }
}

BOOST_AUTO_TEST_CASE( binary_multimap )
{
  test_multimap<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_multimap )
{
  test_multimap<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_multimap )
{
  test_multimap<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_multimap )
{
  test_multimap<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

