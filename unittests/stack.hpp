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
#ifndef CEREAL_TEST_STACK_H_
#define CEREAL_TEST_STACK_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_stack()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::stack<int> o_podstack;
    for(int j=0; j<100; ++j)
      o_podstack.push(random_value<int>(gen));

    std::stack<StructInternalSerialize> o_iserstack;
    for(int j=0; j<100; ++j)
      o_iserstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::stack<StructInternalSplit> o_isplstack;
    for(int j=0; j<100; ++j)
      o_isplstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::stack<StructExternalSerialize> o_eserstack;
    for(int j=0; j<100; ++j)
      o_eserstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::stack<StructExternalSplit> o_esplstack;
    for(int j=0; j<100; ++j)
      o_esplstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podstack);
      oar(o_iserstack);
      oar(o_isplstack);
      oar(o_eserstack);
      oar(o_esplstack);
    }

    std::stack<int> i_podstack;
    std::stack<StructInternalSerialize> i_iserstack;
    std::stack<StructInternalSplit>     i_isplstack;
    std::stack<StructExternalSerialize> i_eserstack;
    std::stack<StructExternalSplit>     i_esplstack;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podstack);
      iar(i_iserstack);
      iar(i_isplstack);
      iar(i_eserstack);
      iar(i_esplstack);
    }

    auto & i_podstack_c  = cereal::stack_detail::container(i_podstack);
    auto & i_iserstack_c = cereal::stack_detail::container(i_iserstack);
    auto & i_isplstack_c = cereal::stack_detail::container(i_isplstack);
    auto & i_eserstack_c = cereal::stack_detail::container(i_eserstack);
    auto & i_esplstack_c = cereal::stack_detail::container(i_esplstack);

    auto & o_podstack_c  = cereal::stack_detail::container(o_podstack);
    auto & o_iserstack_c = cereal::stack_detail::container(o_iserstack);
    auto & o_isplstack_c = cereal::stack_detail::container(o_isplstack);
    auto & o_eserstack_c = cereal::stack_detail::container(o_eserstack);
    auto & o_esplstack_c = cereal::stack_detail::container(o_esplstack);

    check_collection(i_podstack_c,  o_podstack_c );
    check_collection(i_iserstack_c, o_iserstack_c);
    check_collection(i_isplstack_c, o_isplstack_c);
    check_collection(i_eserstack_c, o_eserstack_c);
    check_collection(i_esplstack_c, o_esplstack_c);
  }
}

#endif // CEREAL_TEST_STACK_H_
