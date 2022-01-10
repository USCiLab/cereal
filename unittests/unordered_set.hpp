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
#ifndef CEREAL_TEST_UNORDERED_SET_H_
#define CEREAL_TEST_UNORDERED_SET_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_unordered_set()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::unordered_set<int> o_podunordered_set;
    for(int j=0; j<100; ++j)
      o_podunordered_set.insert(random_value<int>(gen));

    std::unordered_set<StructInternalSerialize, StructHash<StructInternalSerialize>> o_iserunordered_set;
    for(int j=0; j<100; ++j)
      o_iserunordered_set.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::unordered_set<StructInternalSplit, StructHash<StructInternalSplit>> o_isplunordered_set;
    for(int j=0; j<100; ++j)
      o_isplunordered_set.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::unordered_set<StructExternalSerialize, StructHash<StructExternalSerialize>> o_eserunordered_set;
    for(int j=0; j<100; ++j)
      o_eserunordered_set.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::unordered_set<StructExternalSplit, StructHash<StructExternalSplit>> o_esplunordered_set;
    for(int j=0; j<100; ++j)
      o_esplunordered_set.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podunordered_set);
      oar(o_iserunordered_set);
      oar(o_isplunordered_set);
      oar(o_eserunordered_set);
      oar(o_esplunordered_set);
    }

    std::unordered_set<int> i_podunordered_set;
    std::unordered_set<StructInternalSerialize, StructHash<StructInternalSerialize>>   i_iserunordered_set;
    std::unordered_set<StructInternalSplit, StructHash<StructInternalSplit>>           i_isplunordered_set;
    std::unordered_set<StructExternalSerialize, StructHash<StructExternalSerialize>>   i_eserunordered_set;
    std::unordered_set<StructExternalSplit, StructHash<StructExternalSplit>>           i_esplunordered_set;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podunordered_set);
      iar(i_iserunordered_set);
      iar(i_isplunordered_set);
      iar(i_eserunordered_set);
      iar(i_esplunordered_set);
    }

    for(auto const & p : i_podunordered_set)
    {
      CHECK_EQ(o_podunordered_set.count(p), 1lu);
    }

    for(auto const & p : i_iserunordered_set)
    {
      CHECK_EQ(o_iserunordered_set.count(p), 1lu);
    }

    for(auto const & p : i_isplunordered_set)
    {
      CHECK_EQ(o_isplunordered_set.count(p), 1lu);
    }

    for(auto const & p : i_eserunordered_set)
    {
      CHECK_EQ(o_eserunordered_set.count(p), 1lu);
    }

    for(auto const & p : i_esplunordered_set)
    {
      CHECK_EQ(o_esplunordered_set.count(p), 1lu);
    }
  }
}

#endif // CEREAL_TEST_UNORDERED_SET_H_
