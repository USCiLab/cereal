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
#ifndef CEREAL_TEST_MULTISET_H_
#define CEREAL_TEST_MULTISET_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
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

    CHECK_EQ( i_podmultiset.size(), o_podmultiset.size() );
    CHECK_UNARY( std::equal( i_podmultiset.begin(), i_podmultiset.end(), o_podmultiset.begin() ) );
    for(auto const & p : i_podmultiset)
    {
      CHECK_EQ(o_podmultiset.count(p), i_podmultiset.count(p));
    }

    CHECK_EQ( i_isermultiset.size(), o_isermultiset.size() );
    CHECK_UNARY( std::equal( i_isermultiset.begin(), i_isermultiset.end(), o_isermultiset.begin() ) );
    for(auto const & p : i_isermultiset)
    {
      CHECK_EQ(o_isermultiset.count(p), i_isermultiset.count(p));
    }

    CHECK_EQ( i_isplmultiset.size(), o_isplmultiset.size() );
    CHECK_UNARY( std::equal( i_isplmultiset.begin(), i_isplmultiset.end(), o_isplmultiset.begin() ) );
    for(auto const & p : i_isplmultiset)
    {
      CHECK_EQ(o_isplmultiset.count(p), i_isplmultiset.count(p));
    }

    CHECK_EQ( i_esermultiset.size(), o_esermultiset.size() );
    CHECK_UNARY( std::equal( i_esermultiset.begin(), i_esermultiset.end(), o_esermultiset.begin() ) );
    for(auto const & p : i_esermultiset)
    {
      CHECK_EQ(o_esermultiset.count(p), i_esermultiset.count(p));
    }

    CHECK_EQ( i_esplmultiset.size(), o_esplmultiset.size() );
    CHECK_UNARY( std::equal( i_esplmultiset.begin(), i_esplmultiset.end(), o_esplmultiset.begin() ) );
    for(auto const & p : i_esplmultiset)
    {
      CHECK_EQ(o_esplmultiset.count(p), i_esplmultiset.count(p));
    }
  }
}

template <class IArchive, class OArchive> inline
void test_multiset_equivalent_keys_order()
{
  std::multiset<int> o_multiset;
  o_multiset.emplace(1);
  o_multiset.emplace(1);
  o_multiset.emplace(1);
  o_multiset.emplace(2);
  o_multiset.emplace(2);

  std::ostringstream os;
  {
    OArchive oar(os);
    oar(o_multiset);
  }

  std::multiset<int> i_multiset;
  std::istringstream is(os.str());
  {
    IArchive iar(is);
    iar(i_multiset);
  }

  CHECK_EQ(i_multiset.size(), o_multiset.size());
  CHECK_UNARY(std::equal(i_multiset.begin(), i_multiset.end(), o_multiset.begin()));
}

#endif // CEREAL_TEST_MULTISET_H_
