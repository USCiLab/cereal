/*
  Copyright (c) 2014, Klemens D. Morgenstern
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
#include <cereal/types/boost_variant.hpp>

#define BOOST_TEST_MODULE NAMED_VARIANT

#include <boost/test/unit_test.hpp>

template <class IArchive, class OArchive>
void test_array()
{
  std::random_device rd;
  std::mt19937 gen(rd());

    //basic test, if it works in a named way.
    boost::variant<int, double> var1 = 42;
    boost::variant<int, double> var2 = 3.124;

    //This will switch the arguments by misusing the name, to switch the arguments.
    boost::variant<int, unsigned int> var3 = 1;

    std::ostringstream os;
    {
      OArchive oar(os);
      oar ( CEREAL_NVP_NAMED_VARIANT(var1) ) ;//deduces the types automatically.
      oar ( CEREAL_NVP_NAMED_VARIANT(var2));

      oar ( CEREAL_NVP_NAMED_VARIANT(var3, "int", "uint"));
    }

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      //switch them, so I know it was really serialized.
      iar ( CEREAL_NVP_NAMED_VARIANT_("var1" , var2) ) ; //this will not work if the
      iar ( CEREAL_NVP_NAMED_VARIANT_("var2" , var1) ) ;

      iar ( CEREAL_NVP_NAMED_VARIANT(var3, "uint", "int"));

    }

    BOOST_REQUIRE(var1.type() == boost::typeindex::type_id<double>());
    BOOST_CHECK(boost::get<double>(var1) == 3.124);

    BOOST_REQUIRE(var2.type() == boost::typeindex::type_id<int>());
    BOOST_CHECK(boost::get<int>(var2) == 42);

    BOOST_REQUIRE(var3.type() == boost::typeindex::type_id<unsigned int>());
    BOOST_CHECK(boost::get<unsigned int>(var3) == 1);
}

BOOST_AUTO_TEST_CASE( binary_array )
{
  test_array<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_array )
{
  test_array<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_array )
{
  test_array<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_array )
{
  test_array<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


