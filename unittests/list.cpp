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
void test_list()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::list<int> o_podlist(100);
    for(auto & elem : o_podlist)
      elem = random_value<int>(gen);

    std::list<StructInternalSerialize> o_iserlist(100);
    for(auto & elem : o_iserlist)
      elem = StructInternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::list<StructInternalSplit> o_ispllist(100);
    for(auto & elem : o_ispllist)
      elem = StructInternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::list<StructExternalSerialize> o_eserlist(100);
    for(auto & elem : o_eserlist)
      elem = StructExternalSerialize( random_value<int>(gen), random_value<int>(gen) );

    std::list<StructExternalSplit> o_espllist(100);
    for(auto & elem : o_espllist)
      elem = StructExternalSplit( random_value<int>(gen), random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podlist);
      oar(o_iserlist);
      oar(o_ispllist);
      oar(o_eserlist);
      oar(o_espllist);
    }

    std::list<int> i_podlist;
    std::list<StructInternalSerialize> i_iserlist;
    std::list<StructInternalSplit>     i_ispllist;
    std::list<StructExternalSerialize> i_eserlist;
    std::list<StructExternalSplit>     i_espllist;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podlist);
      iar(i_iserlist);
      iar(i_ispllist);
      iar(i_eserlist);
      iar(i_espllist);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podlist.begin(),    i_podlist.end(),    o_podlist.begin(),  o_podlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserlist.begin(),   i_iserlist.end(),   o_iserlist.begin(), o_iserlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_ispllist.begin(),   i_ispllist.end(),   o_ispllist.begin(), o_ispllist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserlist.begin(),   i_eserlist.end(),   o_eserlist.begin(), o_eserlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_espllist.begin(),   i_espllist.end(),   o_espllist.begin(), o_espllist.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_list )
{
  test_list<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_list )
{
  test_list<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_list )
{
  test_list<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_list )
{
  test_list<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

