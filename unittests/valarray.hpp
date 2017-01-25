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
#ifndef CEREAL_TEST_VALARRAY_H_
#define CEREAL_TEST_VALARRAY_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_valarray()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for (int ii = 0; ii<100; ++ii)
  {
    std::valarray<int> o_podvalarray(100);
    for (auto & elem : o_podvalarray)
      elem = random_value<int>(gen);

    std::valarray<StructInternalSerialize> o_iservalarray(100);
    for (auto & elem : o_iservalarray)
      elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

    std::valarray<StructInternalSplit> o_isplvalarray(100);
    for (auto & elem : o_isplvalarray)
      elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

    std::valarray<StructExternalSerialize> o_eservalarray(100);
    for (auto & elem : o_eservalarray)
      elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

    std::valarray<StructExternalSplit> o_esplvalarray(100);
    for (auto & elem : o_esplvalarray)
      elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podvalarray);
      oar(o_iservalarray);
      oar(o_isplvalarray);
      oar(o_eservalarray);
      oar(o_esplvalarray);
    }

    std::valarray<int> i_podvalarray;
    std::valarray<StructInternalSerialize> i_iservalarray;
    std::valarray<StructInternalSplit>     i_isplvalarray;
    std::valarray<StructExternalSerialize> i_eservalarray;
    std::valarray<StructExternalSplit>     i_esplvalarray;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podvalarray);
      iar(i_iservalarray);
      iar(i_isplvalarray);
      iar(i_eservalarray);
      iar(i_esplvalarray);
    }

    CHECK_EQ(i_podvalarray.size(), o_podvalarray.size());
    CHECK_EQ(i_iservalarray.size(), o_iservalarray.size());
    CHECK_EQ(i_isplvalarray.size(), o_isplvalarray.size());
    CHECK_EQ(i_eservalarray.size(), o_eservalarray.size());
    CHECK_EQ(i_esplvalarray.size(), o_esplvalarray.size());

    check_collection(i_podvalarray , o_podvalarray );
    check_collection(i_iservalarray, o_iservalarray);
    check_collection(i_isplvalarray, o_isplvalarray);
    check_collection(i_eservalarray, o_eservalarray);
    check_collection(i_esplvalarray, o_esplvalarray);
  }
}

#endif // CEREAL_TEST_VALARRAY_H_
