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
#ifndef CEREAL_TEST_VECTOR_H_
#define CEREAL_TEST_VECTOR_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_vector()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::vector<int> o_podvector(100);
    for(auto & elem : o_podvector)
      elem = random_value<int>(gen);

    std::vector<bool> o_boolvector; o_boolvector.resize(100);
    for( size_t i = 0; i < 100; ++i )
      o_boolvector[i] = (random_value<int>(gen) % 2) == 0;

    std::vector<StructInternalSerialize> o_iservector(100);
    for(auto & elem : o_iservector)
      elem = StructInternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::vector<StructInternalSplit> o_isplvector(100);
    for(auto & elem : o_isplvector)
      elem = StructInternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::vector<StructExternalSerialize> o_eservector(100);
    for(auto & elem : o_eservector)
      elem = StructExternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::vector<StructExternalSplit> o_esplvector(100);
    for(auto & elem : o_esplvector)
      elem = StructExternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podvector);
      oar(o_boolvector);
      oar(o_iservector);
      oar(o_isplvector);
      oar(o_eservector);
      oar(o_esplvector);
    }

    std::vector<int> i_podvector;
    std::vector<bool> i_boolvector;
    std::vector<StructInternalSerialize> i_iservector;
    std::vector<StructInternalSplit>     i_isplvector;
    std::vector<StructExternalSerialize> i_eservector;
    std::vector<StructExternalSplit>     i_esplvector;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podvector);
      iar(i_boolvector);
      iar(i_iservector);
      iar(i_isplvector);
      iar(i_eservector);
      iar(i_esplvector);
    }

    CHECK_EQ(i_podvector.size(),  o_podvector.size());
    CHECK_EQ(i_boolvector.size(),  o_boolvector.size());
    CHECK_EQ(i_iservector.size(), o_iservector.size());
    CHECK_EQ(i_isplvector.size(), o_isplvector.size());
    CHECK_EQ(i_eservector.size(), o_eservector.size());
    CHECK_EQ(i_esplvector.size(), o_esplvector.size());

    check_collection(i_podvector,  o_podvector );
    check_collection(i_boolvector, o_boolvector);
    check_collection(i_iservector, o_iservector);
    check_collection(i_isplvector, o_isplvector);
    check_collection(i_eservector, o_eservector);
    check_collection(i_esplvector, o_esplvector);
  }
}

#endif // CEREAL_TEST_VECTOR_H_
