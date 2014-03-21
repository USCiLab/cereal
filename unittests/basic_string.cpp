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
void test_string_basic()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    std::basic_string<char> o_string  = random_basic_string<char>(gen);
    std::basic_string<char> o_string2 = "";

    std::ostringstream os;
    {
      OArchive oar(os);
      oar(o_string);
      oar(o_string2);
    }

    std::basic_string<char> i_string;
    std::basic_string<char> i_string2;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_string);
      iar(i_string2);
    }

    BOOST_CHECK_EQUAL(i_string, o_string);
    BOOST_CHECK_EQUAL(i_string2, o_string2);
  }
}

template <class IArchive, class OArchive>
void test_string_all()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    std::basic_string<char> o_string        = random_basic_string<char>(gen);
    std::basic_string<wchar_t> o_wstring    = random_basic_string<wchar_t>(gen);
    std::basic_string<char16_t> o_u16string = random_basic_string<char16_t>(gen);
    std::basic_string<char32_t> o_u32string = random_basic_string<char32_t>(gen);

    std::ostringstream os;
    {
      OArchive oar(os);
      oar(o_string);
      oar(o_wstring);
      oar(o_u16string);
      oar(o_u32string);
    }

    std::basic_string<char> i_string;
    std::basic_string<wchar_t> i_wstring;
    std::basic_string<char16_t> i_u16string;
    std::basic_string<char32_t> i_u32string;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_string);
      iar(i_wstring);
      iar(i_u16string);
      iar(i_u32string);
    }

    BOOST_CHECK_EQUAL(i_string, o_string);
    BOOST_CHECK_EQUAL_COLLECTIONS(i_wstring.begin(),     i_wstring.end(),   o_wstring.begin(),   o_wstring.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_u16string.begin(),  i_u16string.end(),  o_u16string.begin(), o_u16string.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_u32string.begin(),  i_u32string.end(),  o_u32string.begin(), o_u32string.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_string )
{
  test_string_all<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_string )
{
  test_string_all<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_string_basic )
{
  test_string_basic<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_string_basic )
{
  test_string_basic<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

