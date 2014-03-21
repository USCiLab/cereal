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
#include "common.hpp"
#include <boost/test/unit_test.hpp>

template <class IArchive, class OArchive>
void test_queue()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::queue<int> o_podqueue;
    for(int j=0; j<100; ++j)
      o_podqueue.push(random_value<int>(gen));

    std::queue<StructInternalSerialize> o_iserqueue;
    for(int j=0; j<100; ++j)
      o_iserqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::queue<StructInternalSplit> o_isplqueue;
    for(int j=0; j<100; ++j)
      o_isplqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::queue<StructExternalSerialize> o_eserqueue;
    for(int j=0; j<100; ++j)
      o_eserqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::queue<StructExternalSplit> o_esplqueue;
    for(int j=0; j<100; ++j)
      o_esplqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podqueue);
      oar(o_iserqueue);
      oar(o_isplqueue);
      oar(o_eserqueue);
      oar(o_esplqueue);
    }

    std::queue<int> i_podqueue;
    std::queue<StructInternalSerialize> i_iserqueue;
    std::queue<StructInternalSplit>     i_isplqueue;
    std::queue<StructExternalSerialize> i_eserqueue;
    std::queue<StructExternalSplit>     i_esplqueue;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podqueue);
      iar(i_iserqueue);
      iar(i_isplqueue);
      iar(i_eserqueue);
      iar(i_esplqueue);
    }

    auto & i_podqueue_c  = cereal::queue_detail::container(i_podqueue);
    auto & i_iserqueue_c = cereal::queue_detail::container(i_iserqueue);
    auto & i_isplqueue_c = cereal::queue_detail::container(i_isplqueue);
    auto & i_eserqueue_c = cereal::queue_detail::container(i_eserqueue);
    auto & i_esplqueue_c = cereal::queue_detail::container(i_esplqueue);

    auto & o_podqueue_c  = cereal::queue_detail::container(o_podqueue);
    auto & o_iserqueue_c = cereal::queue_detail::container(o_iserqueue);
    auto & o_isplqueue_c = cereal::queue_detail::container(o_isplqueue);
    auto & o_eserqueue_c = cereal::queue_detail::container(o_eserqueue);
    auto & o_esplqueue_c = cereal::queue_detail::container(o_esplqueue);

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podqueue_c.begin(),    i_podqueue_c.end(),    o_podqueue_c.begin(),  o_podqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserqueue_c.begin(),   i_iserqueue_c.end(),   o_iserqueue_c.begin(), o_iserqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplqueue_c.begin(),   i_isplqueue_c.end(),   o_isplqueue_c.begin(), o_isplqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserqueue_c.begin(),   i_eserqueue_c.end(),   o_eserqueue_c.begin(), o_eserqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplqueue_c.begin(),   i_esplqueue_c.end(),   o_esplqueue_c.begin(), o_esplqueue_c.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_queue )
{
  test_queue<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_queue )
{
  test_queue<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_queue )
{
  test_queue<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_queue )
{
  test_queue<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

