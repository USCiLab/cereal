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
#ifndef CEREAL_TEST_PAIR_H_
#define CEREAL_TEST_PAIR_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_pair()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    std::pair<int, int> o_podpair = {rng(), rng()};
    std::pair<StructInternalSerialize, StructInternalSerialize> o_iserpair = {{rng(), rng()}, {rng(), rng()}};
    std::pair<StructInternalSplit, StructInternalSplit> o_isplpair = {{rng(), rng()}, {rng(), rng()}};
    std::pair<StructExternalSerialize, StructExternalSerialize> o_eserpair = {{rng(), rng()}, {rng(), rng()}};
    std::pair<StructExternalSplit, StructExternalSplit> o_esplpair = {{rng(), rng()}, {rng(), rng()}};

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podpair);
      oar(o_iserpair);
      oar(o_isplpair);
      oar(o_eserpair);
      oar(o_esplpair);
    }

    std::pair<int, int> i_podpair;
    std::pair<StructInternalSerialize, StructInternalSerialize> i_iserpair;
    std::pair<StructInternalSplit, StructInternalSplit> i_isplpair;
    std::pair<StructExternalSerialize, StructExternalSerialize> i_eserpair;
    std::pair<StructExternalSplit, StructExternalSplit> i_esplpair;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podpair);
      iar(i_iserpair);
      iar(i_isplpair);
      iar(i_eserpair);
      iar(i_esplpair);
    }

    CHECK_EQ( i_podpair.first, o_podpair.first );
    CHECK_EQ( i_podpair.second, o_podpair.second );

    CHECK_EQ( i_iserpair.first, o_iserpair.first );
    CHECK_EQ( i_iserpair.second, o_iserpair.second );

    CHECK_EQ( i_isplpair.first, o_isplpair.first );
    CHECK_EQ( i_isplpair.second, o_isplpair.second );

    CHECK_EQ( i_eserpair.first, o_eserpair.first );
    CHECK_EQ( i_eserpair.second, o_eserpair.second );

    CHECK_EQ( i_esplpair.first, o_esplpair.first );
    CHECK_EQ( i_esplpair.second, o_esplpair.second );
  }
}

#endif // CEREAL_TEST_PAIR_H_
