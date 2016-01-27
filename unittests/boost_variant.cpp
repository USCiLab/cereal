/*
  Copyright (c) 2015, Kyle Fleming
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
void test_boost_variant()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  boost::variant<int, double, std::string> o_bv1 = random_value<int>(gen);
  boost::variant<int, double, std::string> o_bv2 = random_value<double>(gen);
  boost::variant<int, double, std::string> o_bv3 = random_basic_string<char>(gen);

  std::ostringstream os;
  {
    OArchive oar(os);

    oar(o_bv1);
    oar(o_bv2);
    oar(o_bv3);
  }

  decltype(o_bv1) i_bv1;
  decltype(o_bv2) i_bv2;
  decltype(o_bv3) i_bv3;

  std::istringstream is(os.str());
  {
    IArchive iar(is);

    iar(i_bv1);
    iar(i_bv2);
    iar(i_bv3);
  }

  BOOST_CHECK_EQUAL( boost::get<int>(i_bv1), boost::get<int>(o_bv1) );
  BOOST_CHECK_CLOSE( boost::get<double>(i_bv2), boost::get<double>(o_bv2), 1e-5 );
  BOOST_CHECK_EQUAL( boost::get<std::string>(i_bv3), boost::get<std::string>(o_bv3) );
}

BOOST_AUTO_TEST_CASE( binary_boost_variant )
{
  test_boost_variant<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_boost_variant )
{
  test_boost_variant<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_boost_variant )
{
  test_boost_variant<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_boost_variant )
{
  test_boost_variant<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


