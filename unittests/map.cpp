/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES AND SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "common.hpp"
#include <boost/test/unit_test.hpp>

template <class IArchive, class OArchive>
void test_map()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::map<size_t, std::vector<StructInternalSerialize>> o_vectormap;
    for(int j=0; j<10; ++j)
    {
      size_t id = random_value<size_t>(gen);
      for(int k=0; k<100; ++k)
        o_vectormap[id].emplace_back(random_value<int>(gen), random_value<int>(gen));
    }

    std::map<std::string, int> o_podmap;
    for(int j=0; j<100; ++j)
      o_podmap.insert({random_value<std::string>(gen), random_value<int>(gen)});

    std::map<int, StructInternalSerialize> o_isermap;
    for(int j=0; j<100; ++j)
      o_isermap.insert({random_value<int>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::map<int, StructInternalSplit> o_isplmap;
    for(int j=0; j<100; ++j)
      o_isplmap.insert({random_value<int>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::map<uint32_t, StructExternalSerialize> o_esermap;
    for(int j=0; j<100; ++j)
      o_esermap.insert({random_value<uint32_t>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::map<int8_t, StructExternalSplit> o_esplmap;
    for(int j=0; j<100; ++j)
      o_esplmap.insert({random_value<char>(gen),  { random_value<int>(gen), random_value<int>(gen) }});

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_vectormap);
      oar(o_podmap);
      oar(o_isermap);
      oar(o_isplmap);
      oar(o_esermap);
      oar(o_esplmap);
    }

    std::map<size_t, std::vector<StructInternalSerialize>> i_vectormap;
    std::map<std::string, int> i_podmap;
    std::map<int, StructInternalSerialize>   i_isermap;
    std::map<int, StructInternalSplit>        i_isplmap;
    std::map<uint32_t, StructExternalSerialize> i_esermap;
    std::map<int8_t, StructExternalSplit>       i_esplmap;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_vectormap);
      iar(i_podmap);
      iar(i_isermap);
      iar(i_isplmap);
      iar(i_esermap);
      iar(i_esplmap);
    }

    BOOST_CHECK_EQUAL(i_vectormap.size(), o_vectormap.size());
    auto o_v_it = o_vectormap.begin();
    auto i_v_it = i_vectormap.begin();
    for(;o_v_it != o_vectormap.end(); ++o_v_it, ++i_v_it)
    {
      BOOST_CHECK_EQUAL(i_v_it->second.size(), o_v_it->second.size());
      BOOST_CHECK_EQUAL_COLLECTIONS(i_v_it->second.begin(), i_v_it->second.end(), o_v_it->second.begin(), o_v_it->second.end());
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podmap.begin(),    i_podmap.end(),    o_podmap.begin(),  o_podmap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isermap.begin(),   i_isermap.end(),   o_isermap.begin(), o_isermap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplmap.begin(),   i_isplmap.end(),   o_isplmap.begin(), o_isplmap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esermap.begin(),   i_esermap.end(),   o_esermap.begin(), o_esermap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplmap.begin(),   i_esplmap.end(),   o_esplmap.begin(), o_esplmap.end());
  }
}

template <class IArchive, class OArchive>
void test_map_memory()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::map<int, std::unique_ptr<int>> o_uniqueptrMap;
    std::map<int, std::shared_ptr<int>> o_sharedptrMap;

    for(int j=0; j<100; ++j)
    {
      #ifdef CEREAL_OLDER_GCC
      o_uniqueptrMap.insert( std::make_pair(random_value<int>(gen), std::unique_ptr<int>( new int( random_value<int>(gen) ) )) );
      o_sharedptrMap.insert( std::make_pair(random_value<int>(gen), std::make_shared<int>( random_value<int>(gen) )) );
      #else // NOT CEREAL_OLDER_GCC
      o_uniqueptrMap.emplace( random_value<int>(gen), std::unique_ptr<int>( new int( random_value<int>(gen) ) ) );
      o_sharedptrMap.emplace( random_value<int>(gen), std::make_shared<int>( random_value<int>(gen) ) );
      #endif // NOT CEREAL_OLDER_GCC
    }

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_uniqueptrMap );
      oar( o_sharedptrMap );
    }

    decltype( o_uniqueptrMap ) i_uniqueptrMap;
    decltype( o_sharedptrMap ) i_sharedptrMap;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_uniqueptrMap );
      iar( i_sharedptrMap );
    }

    BOOST_CHECK_EQUAL(o_sharedptrMap.size(), i_sharedptrMap.size());
    BOOST_CHECK_EQUAL(o_uniqueptrMap.size(), i_uniqueptrMap.size());

    auto o_v_it = o_uniqueptrMap.begin();
    auto i_v_it = i_uniqueptrMap.begin();
    for(;o_v_it != o_uniqueptrMap.end(); ++o_v_it, ++i_v_it)
    {
      BOOST_CHECK_EQUAL(i_v_it->first, o_v_it->first);
      BOOST_CHECK_EQUAL(*i_v_it->second, *o_v_it->second);
    }
  }
}

BOOST_AUTO_TEST_CASE( binary_map )
{
  test_map<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_map )
{
  test_map<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_map )
{
  test_map<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_map )
{
  test_map<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

BOOST_AUTO_TEST_CASE( binary_map_memory )
{
  test_map_memory<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_map_memory )
{
  test_map_memory<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_map_memory )
{
  test_map_memory<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_map_memory )
{
  test_map_memory<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
