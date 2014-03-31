/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant, Joel Holdsworth
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
void test_concatenation()
{
  // Strings with newlines are being used to check that input archives to not
  // treat them incorrectly as document terminators
  std::basic_string<char> o_string  = "Apples\n\nPears";
  std::basic_string<char> o_string2 = "Oranges\n\nLemons";

  // Create a stream containing to concatenated archives
  std::ostringstream os;
  {
    OArchive oar(os);
    oar(o_string);
  }
  {
    OArchive oar(os);
    oar(o_string2);
  }

  std::basic_string<char> i_string;
  std::basic_string<char> i_string2;

  // Now try and read them back
  std::istringstream is(os.str());
  {
    IArchive iar(is);
    iar(i_string);
  }
  {
    IArchive iar(is);
    iar(i_string2);
  }

  BOOST_CHECK_EQUAL(i_string, o_string);
  BOOST_CHECK_EQUAL(i_string2, o_string2);
}

BOOST_AUTO_TEST_CASE( binary_string )
{
  test_concatenation<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_string )
{
  test_concatenation<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

#if 0
BOOST_AUTO_TEST_CASE( xml_string_basic )
{
  test_concatenation<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_string_basic )
{
  test_concatenation<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
#endif
