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
#ifndef CEREAL_TEST_UNORDERED_MULTISET_H_
#define CEREAL_TEST_UNORDERED_MULTISET_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_unordered_multiset()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::unordered_multiset<int> o_podunordered_multiset;
    for(int j=0; j<100; ++j)
    {
      int value = random_value<int>(gen);
      o_podunordered_multiset.insert(value);
      o_podunordered_multiset.insert(value);
    }

    std::unordered_multiset<StructInternalSerialize, StructHash<StructInternalSerialize>> o_iserunordered_multiset;
    for(int j=0; j<100; ++j)
    {
      StructInternalSerialize value = { random_value<int>(gen), random_value<int>(gen) };
      o_iserunordered_multiset.insert(value);
      o_iserunordered_multiset.insert(value);
    }

    std::unordered_multiset<StructInternalSplit, StructHash<StructInternalSplit>> o_isplunordered_multiset;
    for(int j=0; j<100; ++j)
    {
      StructInternalSplit value = { random_value<int>(gen), random_value<int>(gen) };
      o_isplunordered_multiset.insert(value);
      o_isplunordered_multiset.insert(value);
    }

    std::unordered_multiset<StructExternalSerialize, StructHash<StructExternalSerialize>> o_eserunordered_multiset;
    for(int j=0; j<100; ++j)
    {
      StructExternalSerialize value = { random_value<int>(gen), random_value<int>(gen) };
      o_eserunordered_multiset.insert(value);
      o_eserunordered_multiset.insert(value);
    }

    std::unordered_multiset<StructExternalSplit, StructHash<StructExternalSplit>> o_esplunordered_multiset;
    for(int j=0; j<100; ++j)
    {
      StructExternalSplit value = { random_value<int>(gen), random_value<int>(gen) };
      o_esplunordered_multiset.insert(value);
      o_esplunordered_multiset.insert(value);
    }

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podunordered_multiset);
      oar(o_iserunordered_multiset);
      oar(o_isplunordered_multiset);
      oar(o_eserunordered_multiset);
      oar(o_esplunordered_multiset);
    }

    std::unordered_multiset<int> i_podunordered_multiset;
    std::unordered_multiset<StructInternalSerialize, StructHash<StructInternalSerialize>> i_iserunordered_multiset;
    std::unordered_multiset<StructInternalSplit, StructHash<StructInternalSplit>>         i_isplunordered_multiset;
    std::unordered_multiset<StructExternalSerialize, StructHash<StructExternalSerialize>> i_eserunordered_multiset;
    std::unordered_multiset<StructExternalSplit, StructHash<StructExternalSplit>>         i_esplunordered_multiset;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podunordered_multiset);
      iar(i_iserunordered_multiset);
      iar(i_isplunordered_multiset);
      iar(i_eserunordered_multiset);
      iar(i_esplunordered_multiset);
    }

    for(auto const & p : i_podunordered_multiset)
    {
      CHECK_EQ(o_podunordered_multiset.count(p), i_podunordered_multiset.count(p));
    }

    for(auto const & p : i_iserunordered_multiset)
    {
      CHECK_EQ(o_iserunordered_multiset.count(p), i_iserunordered_multiset.count(p));
    }

    for(auto const & p : i_isplunordered_multiset)
    {
      CHECK_EQ(o_isplunordered_multiset.count(p), i_isplunordered_multiset.count(p));
    }

    for(auto const & p : i_eserunordered_multiset)
    {
      CHECK_EQ(o_eserunordered_multiset.count(p), i_eserunordered_multiset.count(p));
    }

    for(auto const & p : i_esplunordered_multiset)
    {
      CHECK_EQ(o_esplunordered_multiset.count(p), i_esplunordered_multiset.count(p));
    }
  }
}

#endif // CEREAL_TEST_UNORDERED_MULTISET_H_
