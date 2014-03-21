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
void test_unordered_map()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::unordered_map<std::string, int> o_podunordered_map;
    for(int j=0; j<100; ++j)
      o_podunordered_map.insert({random_value<std::string>(gen), random_value<int>(gen)});

    std::unordered_map<uint16_t, StructInternalSerialize> o_iserunordered_map;
    for(int j=0; j<100; ++j)
      o_iserunordered_map.insert({random_value<uint16_t>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::unordered_map<uint16_t, StructInternalSplit> o_isplunordered_map;
    for(int j=0; j<100; ++j)
      o_isplunordered_map.insert({random_value<uint16_t>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::unordered_map<uint32_t, StructExternalSerialize> o_eserunordered_map;
    for(int j=0; j<100; ++j)
      o_eserunordered_map.insert({random_value<uint32_t>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::unordered_map<int8_t, StructExternalSplit> o_esplunordered_map;
    for(int j=0; j<100; ++j)
      o_esplunordered_map.insert({random_value<char>(gen),  { random_value<int>(gen), random_value<int>(gen) }});

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podunordered_map);
      oar(o_iserunordered_map);
      oar(o_isplunordered_map);
      oar(o_eserunordered_map);
      oar(o_esplunordered_map);
    }

    std::unordered_map<std::string, int> i_podunordered_map;
    std::unordered_map<uint16_t, StructInternalSerialize>   i_iserunordered_map;
    std::unordered_map<uint16_t, StructInternalSplit>        i_isplunordered_map;
    std::unordered_map<uint32_t, StructExternalSerialize> i_eserunordered_map;
    std::unordered_map<int8_t, StructExternalSplit>       i_esplunordered_map;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podunordered_map);
      iar(i_iserunordered_map);
      iar(i_isplunordered_map);
      iar(i_eserunordered_map);
      iar(i_esplunordered_map);
    }

    for(auto const & p : i_podunordered_map)
    {
      auto v = o_podunordered_map.find(p.first);
      BOOST_CHECK(v != o_podunordered_map.end());
      BOOST_CHECK_EQUAL(p.second, v->second);
    }

    for(auto const & p : i_iserunordered_map)
    {
      auto v = o_iserunordered_map.find(p.first);
      BOOST_CHECK(v != o_iserunordered_map.end());
      BOOST_CHECK_EQUAL(p.second, v->second);
    }

    for(auto const & p : i_isplunordered_map)
    {
      auto v = o_isplunordered_map.find(p.first);
      BOOST_CHECK(v != o_isplunordered_map.end());
      BOOST_CHECK_EQUAL(p.second, v->second);
    }

    for(auto const & p : i_eserunordered_map)
    {
      auto v = o_eserunordered_map.find(p.first);
      BOOST_CHECK(v != o_eserunordered_map.end());
      BOOST_CHECK_EQUAL(p.second, v->second);
    }

    for(auto const & p : i_esplunordered_map)
    {
      auto v = o_esplunordered_map.find(p.first);
      BOOST_CHECK(v != o_esplunordered_map.end());
      BOOST_CHECK_EQUAL(p.second, v->second);
    }
  }
}

BOOST_AUTO_TEST_CASE( binary_unordered_map )
{
  test_unordered_map<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_unordered_map )
{
  test_unordered_map<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_unordered_map )
{
  test_unordered_map<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_unordered_map )
{
  test_unordered_map<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

