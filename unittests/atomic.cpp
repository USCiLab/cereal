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
void test_atomic()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::atomic<bool> o_ab(std::uniform_int_distribution<int>(0,1)(gen));
    std::atomic<signed char> o_asc(std::uniform_int_distribution<signed char>(
      std::numeric_limits<signed char>::min(),
      std::numeric_limits<signed char>::max()
    )(gen));
    std::atomic<unsigned short> o_aus(std::uniform_int_distribution<unsigned short>(
      std::numeric_limits<unsigned short>::min(),
      std::numeric_limits<unsigned short>::max()
    )(gen));
    std::atomic<int> o_asi(std::uniform_int_distribution<int>(
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::max()
    )(gen));
    std::atomic<long> o_asl(std::uniform_int_distribution<long>(
      std::numeric_limits<long>::min(),
      std::numeric_limits<long>::max()
    )(gen));
    std::atomic<unsigned long long> o_aull(std::uniform_int_distribution<unsigned long long>(
      std::numeric_limits<unsigned long long>::min(),
      std::numeric_limits<unsigned long long>::max()
    )(gen));

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_ab);
      oar(o_asc);
      oar(o_aus);
      oar(o_asi);
      oar(o_asl);
      oar(o_aull);
    }

    std::atomic<bool> i_ab;
    std::atomic<signed char> i_asc;
    std::atomic<unsigned short> i_aus;
    std::atomic<int> i_asi;
    std::atomic<long> i_asl;
    std::atomic<unsigned long long> i_aull;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_ab);
      iar(i_asc);
      iar(i_aus);
      iar(i_asi);
      iar(i_asl);
      iar(i_aull);
    }

    BOOST_CHECK_EQUAL(o_ab.load(),i_ab.load());
    BOOST_CHECK_EQUAL(o_asc.load(),i_asc.load());
    BOOST_CHECK_EQUAL(o_aus.load(),i_aus.load());
    BOOST_CHECK_EQUAL(o_asi.load(),i_asi.load());
    BOOST_CHECK_EQUAL(o_asl.load(),i_asl.load());
    BOOST_CHECK_EQUAL(o_aull.load(),i_aull.load());
  }
}

BOOST_AUTO_TEST_CASE( binary_atomic )
{
  test_atomic<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_atomic )
{
  test_atomic<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_atomic )
{
  test_atomic<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_atomic )
{
  test_atomic<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
