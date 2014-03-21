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

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podset.begin(),    i_podset.end(),    o_podset.begin(),  o_podset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserset.begin(),   i_iserset.end(),   o_iserset.begin(), o_iserset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplset.begin(),   i_isplset.end(),   o_isplset.begin(), o_isplset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserset.begin(),   i_eserset.end(),   o_eserset.begin(), o_eserset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplset.begin(),   i_esplset.end(),   o_esplset.begin(), o_esplset.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_set )
{
  test_set<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_set )
{
  test_set<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_set )
{
  test_set<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_set )
{
  test_set<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

