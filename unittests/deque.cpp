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

    BOOST_CHECK_EQUAL(i_poddeque.size(),  o_poddeque.size());
    BOOST_CHECK_EQUAL(i_iserdeque.size(), o_iserdeque.size());
    BOOST_CHECK_EQUAL(i_ispldeque.size(), o_ispldeque.size());
    BOOST_CHECK_EQUAL(i_eserdeque.size(), o_eserdeque.size());
    BOOST_CHECK_EQUAL(i_espldeque.size(), o_espldeque.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(i_poddeque.begin(),    i_poddeque.end(),    o_poddeque.begin(),  o_poddeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserdeque.begin(),   i_iserdeque.end(),   o_iserdeque.begin(), o_iserdeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_ispldeque.begin(),   i_ispldeque.end(),   o_ispldeque.begin(), o_ispldeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserdeque.begin(),   i_eserdeque.end(),   o_eserdeque.begin(), o_eserdeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_espldeque.begin(),   i_espldeque.end(),   o_espldeque.begin(), o_espldeque.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_dequeue )
{
  test_deque<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_dequeue )
{
  test_deque<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_dequeue )
{
  test_deque<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_dequeue )
{
  test_deque<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
