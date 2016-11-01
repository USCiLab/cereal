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
#ifndef CEREAL_TEST_DEQUE_H_
#define CEREAL_TEST_DEQUE_H_
#include "common.hpp"

template <class IArchive, class OArchive>
void test_deque()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::deque<int> o_poddeque(100);
    for(auto & elem : o_poddeque)
      elem = random_value<int>(gen);

    std::deque<StructInternalSerialize> o_iserdeque(100);
    for(auto & elem : o_iserdeque)
      elem = StructInternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::deque<StructInternalSplit> o_ispldeque(100);
    for(auto & elem : o_ispldeque)
      elem = StructInternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::deque<StructExternalSerialize> o_eserdeque(100);
    for(auto & elem : o_eserdeque)
      elem = StructExternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::deque<StructExternalSplit> o_espldeque(100);
    for(auto & elem : o_espldeque)
      elem = StructExternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_poddeque);
      oar(o_iserdeque);
      oar(o_ispldeque);
      oar(o_eserdeque);
      oar(o_espldeque);
    }

    std::deque<int> i_poddeque;
    std::deque<StructInternalSerialize> i_iserdeque;
    std::deque<StructInternalSplit>     i_ispldeque;
    std::deque<StructExternalSerialize> i_eserdeque;
    std::deque<StructExternalSplit>     i_espldeque;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_poddeque);
      iar(i_iserdeque);
      iar(i_ispldeque);
      iar(i_eserdeque);
      iar(i_espldeque);
    }

    CHECK_EQ(i_poddeque.size(),  o_poddeque.size());
    CHECK_EQ(i_iserdeque.size(), o_iserdeque.size());
    CHECK_EQ(i_ispldeque.size(), o_ispldeque.size());
    CHECK_EQ(i_eserdeque.size(), o_eserdeque.size());
    CHECK_EQ(i_espldeque.size(), o_espldeque.size());

    check_collection(i_poddeque,  o_poddeque );
    check_collection(i_iserdeque, o_iserdeque);
    check_collection(i_ispldeque, o_ispldeque);
    check_collection(i_eserdeque, o_eserdeque);
    check_collection(i_espldeque, o_espldeque);
  }
}

#endif // CEREAL_TEST_DEQUE_H_
