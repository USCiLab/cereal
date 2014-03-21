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
void test_unordered_multimap()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::unordered_multimap<std::string, int> o_podunordered_multimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<std::string>(gen);
      o_podunordered_multimap.insert({key, random_value<int>(gen)});
      o_podunordered_multimap.insert({key, random_value<int>(gen)});
    }

    std::unordered_multimap<int, StructInternalSerialize> o_iserunordered_multimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<int>(gen);
      o_iserunordered_multimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
      o_iserunordered_multimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::unordered_multimap<int, StructInternalSplit> o_isplunordered_multimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<int>(gen);
      o_isplunordered_multimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
      o_isplunordered_multimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::unordered_multimap<uint32_t, StructExternalSerialize> o_eserunordered_multimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<uint32_t>(gen);
      o_eserunordered_multimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
      o_eserunordered_multimap.insert({key, { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::unordered_multimap<int8_t, StructExternalSplit> o_esplunordered_multimap;
    for(int j=0; j<100; ++j)
    {
      auto key = random_value<char>(gen);
      o_esplunordered_multimap.insert({key,  { random_value<int>(gen), random_value<int>(gen) }});
      o_esplunordered_multimap.insert({key,  { random_value<int>(gen), random_value<int>(gen) }});
    }

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podunordered_multimap);
      oar(o_iserunordered_multimap);
      oar(o_isplunordered_multimap);
      oar(o_eserunordered_multimap);
      oar(o_esplunordered_multimap);
    }

    std::unordered_multimap<std::string, int> i_podunordered_multimap;
    std::unordered_multimap<int, StructInternalSerialize>   i_iserunordered_multimap;
    std::unordered_multimap<int, StructInternalSplit>        i_isplunordered_multimap;
    std::unordered_multimap<uint32_t, StructExternalSerialize> i_eserunordered_multimap;
    std::unordered_multimap<int8_t, StructExternalSplit>       i_esplunordered_multimap;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podunordered_multimap);
      iar(i_iserunordered_multimap);
      iar(i_isplunordered_multimap);
      iar(i_eserunordered_multimap);
      iar(i_esplunordered_multimap);
    }

    BOOST_CHECK_EQUAL(i_podunordered_multimap.size(),  o_podunordered_multimap.size());
    BOOST_CHECK_EQUAL(i_iserunordered_multimap.size(), o_iserunordered_multimap.size());
    BOOST_CHECK_EQUAL(i_isplunordered_multimap.size(), o_isplunordered_multimap.size());
    BOOST_CHECK_EQUAL(i_eserunordered_multimap.size(), o_eserunordered_multimap.size());
    BOOST_CHECK_EQUAL(i_esplunordered_multimap.size(), o_esplunordered_multimap.size());

    for(auto const & p : i_podunordered_multimap)
    {
      size_t const bucket = o_podunordered_multimap.bucket(p.first);
      auto bucket_begin   = o_podunordered_multimap.begin(bucket);
      auto bucket_end     = o_podunordered_multimap.end(bucket);
      BOOST_CHECK(std::find(bucket_begin, bucket_end, p) != bucket_end);
    }

    for(auto const & p : i_iserunordered_multimap)
    {
      size_t const bucket = o_iserunordered_multimap.bucket(p.first);
      auto bucket_begin   = o_iserunordered_multimap.begin(bucket);
      auto bucket_end     = o_iserunordered_multimap.end(bucket);
      BOOST_CHECK(std::find(bucket_begin, bucket_end, p) != bucket_end);
    }

    for(auto const & p : i_isplunordered_multimap)
    {
      size_t const bucket = o_isplunordered_multimap.bucket(p.first);
      auto bucket_begin   = o_isplunordered_multimap.begin(bucket);
      auto bucket_end     = o_isplunordered_multimap.end(bucket);
      BOOST_CHECK(std::find(bucket_begin, bucket_end, p) != bucket_end);
    }

    for(auto const & p : i_eserunordered_multimap)
    {
      size_t const bucket = o_eserunordered_multimap.bucket(p.first);
      auto bucket_begin   = o_eserunordered_multimap.begin(bucket);
      auto bucket_end     = o_eserunordered_multimap.end(bucket);
      BOOST_CHECK(std::find(bucket_begin, bucket_end, p) != bucket_end);
    }

    for(auto const & p : i_esplunordered_multimap)
    {
      size_t const bucket = o_esplunordered_multimap.bucket(p.first);
      auto bucket_begin   = o_esplunordered_multimap.begin(bucket);
      auto bucket_end     = o_esplunordered_multimap.end(bucket);
      BOOST_CHECK(std::find(bucket_begin, bucket_end, p) != bucket_end);
    }
  }
}

BOOST_AUTO_TEST_CASE( binary_unordered_multimap )
{
  test_unordered_multimap<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_unordered_multimap )
{
  test_unordered_multimap<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_unordered_multimap )
{
  test_unordered_multimap<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_unordered_multimap )
{
  test_unordered_multimap<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
