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
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TEST_MULTIMAP_H_
#define CEREAL_TEST_MULTIMAP_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
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
      CHECK_EQ( count, OutMap.count( pair.first ) ); \
      auto find = InMap.find( pair.first ); \
      bool found = false; \
      for( size_t i = 0; i < count; ++i, ++find ) \
        found |= find->second == pair.second; \
      CHECK_UNARY( found ); \
    }

    MULTIMAP_CHECK( i_podmultimap, o_podmultimap );
    MULTIMAP_CHECK( i_isermultimap, o_isermultimap );
    MULTIMAP_CHECK( i_isplmultimap, o_isplmultimap );
    MULTIMAP_CHECK( i_esermultimap, o_esermultimap );
    MULTIMAP_CHECK( i_esplmultimap, o_esplmultimap );

#undef MULTIMAP_CHECK
  }
}

#endif // CEREAL_TEST_MULTIMAP_H_
