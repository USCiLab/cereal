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
#ifndef CEREAL_TEST_SET_H_
#define CEREAL_TEST_SET_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_set()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::set<int> o_podset;
    for(int j=0; j<100; ++j)
      o_podset.insert(random_value<int>(gen));

    std::set<StructInternalSerialize> o_iserset;
    for(int j=0; j<100; ++j)
      o_iserset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructInternalSplit> o_isplset;
    for(int j=0; j<100; ++j)
      o_isplset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructExternalSerialize> o_eserset;
    for(int j=0; j<100; ++j)
      o_eserset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructExternalSplit> o_esplset;
    for(int j=0; j<100; ++j)
      o_esplset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podset);
      oar(o_iserset);
      oar(o_isplset);
      oar(o_eserset);
      oar(o_esplset);
    }

    std::set<int> i_podset;
    std::set<StructInternalSerialize>   i_iserset;
    std::set<StructInternalSplit>        i_isplset;
    std::set<StructExternalSerialize> i_eserset;
    std::set<StructExternalSplit>       i_esplset;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podset);
      iar(i_iserset);
      iar(i_isplset);
      iar(i_eserset);
      iar(i_esplset);
    }

    check_collection(i_podset,  o_podset);
    check_collection(i_iserset, o_iserset);
    check_collection(i_isplset, o_isplset);
    check_collection(i_eserset, o_eserset);
    check_collection(i_esplset, o_esplset);
  }
}

#endif // CEREAL_TEST_SET_H_
