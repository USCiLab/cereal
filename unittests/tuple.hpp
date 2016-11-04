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
#ifndef CEREAL_TEST_TUPLE_H_
#define CEREAL_TEST_TUPLE_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_tuple()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    auto o_podtuple = std::make_tuple( rng(), rng(), rng(), rng() );
    auto o_podtuple11 = std::make_tuple( rng(), rng(), rng(), rng(), rng(), rng(),
                                         rng(), rng(), rng(), rng(), rng() );
    auto o_isertuple = std::make_tuple( StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ) );
    auto o_ispltuple = std::make_tuple( StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ) );
    auto o_esertuple = std::make_tuple( StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ) );
    auto o_espltuple = std::make_tuple( StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podtuple);
      oar(o_podtuple11);
      oar(o_isertuple);
      oar(o_ispltuple);
      oar(o_esertuple);
      oar(o_espltuple);
    }

    decltype( o_podtuple   ) i_podtuple;
    decltype( o_podtuple11 ) i_podtuple11;
    decltype( o_isertuple  ) i_isertuple;
    decltype( o_ispltuple  ) i_ispltuple;
    decltype( o_esertuple  ) i_esertuple;
    decltype( o_espltuple  ) i_espltuple;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podtuple);
      iar(i_podtuple11);
      iar(i_isertuple);
      iar(i_ispltuple);
      iar(i_esertuple);
      iar(i_espltuple);
    }

    CHECK_EQ( i_podtuple,   o_podtuple);
    CHECK_EQ( i_podtuple11, o_podtuple11);
    CHECK_EQ( i_isertuple,  o_isertuple);
    CHECK_EQ( i_ispltuple,  o_ispltuple);
    CHECK_EQ( i_esertuple,  o_esertuple);
    CHECK_EQ( i_espltuple,  o_espltuple);
  }
}

#endif // CEREAL_TEST_TUPLE_H_
