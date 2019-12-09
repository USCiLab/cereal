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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "basic_string.hpp"

TEST_SUITE_BEGIN("basic_string");

TEST_CASE("binary_string")
{
  test_string_all<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

TEST_CASE("portable_binary_string")
{
  test_string_all<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

TEST_CASE("xml_string_basic")
{
  test_string_basic<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

TEST_CASE("json_string_basic")
{
  test_string_basic<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

template <class IArchive, class OArchive, class Out, class In = Out>
void test_ws_in_out(Out const & o_value_with_ws)
{
  std::ostringstream os;
  {
    OArchive oar(os);
    oar(o_value_with_ws);
  }

  In i_value_with_ws;

  std::istringstream is(os.str());
  {
    IArchive iar(is);
    iar(i_value_with_ws);
  }

  CHECK(i_value_with_ws == o_value_with_ws);
}

TEST_CASE("xml_string_issue109")
{
  char strings[][20] = {
    "some text",
    "some text ",
    " some text",
    " some text ",
    "  ",
    "    text    ",
    " ]]> ",
    " &gt; > ]]> ",
    " < <]>] &lt; ",
    " &amp; &   "
  };

  for( size_t i=0; i<( sizeof( strings ) / sizeof( strings[0] ) ); ++i )
  {
    std::basic_string<char> o_string = strings[i];

    test_ws_in_out<cereal::XMLInputArchive, cereal::XMLOutputArchive>( o_string );
  }
}

TEST_CASE("xml_char_issue109")
{
  uint8_t chars[] = {
    ' ',
    '\t',
    '\n',
    '\r',
    '&',
    '>',
    '<',
    '\'',
    '"',
    '!',
    '|'
  };

  for( size_t i=0; i<( sizeof( chars ) / sizeof( chars[0] ) ); ++i )
  {
    test_ws_in_out<cereal::XMLInputArchive, cereal::XMLOutputArchive>( chars[i] );
  }

  for( size_t i=0; i<( sizeof( chars ) / sizeof( chars[0] ) ); ++i )
  {
    test_ws_in_out<cereal::XMLInputArchive, cereal::XMLOutputArchive>( int8_t( chars[i] ) );
  }

  for( size_t i=0; i<( sizeof( chars ) / sizeof( chars[0] ) ); ++i )
  {
    test_ws_in_out<cereal::XMLInputArchive, cereal::XMLOutputArchive>( char( chars[i] ) );
  }
}

template <class IArchive, class OArchive, class Out, size_t Nb, class In = Out>
void test_ws_in_out_array(Out const (&o_a_value_with_ws)[Nb])
{
    std::ostringstream os;
    {
        OArchive oar(os);
        for (const auto& o_value_with_ws : o_a_value_with_ws)
        {
            oar(o_value_with_ws);
        }
    }

    In i_a_value_with_ws[Nb];

    std::istringstream is(os.str());
    {
        IArchive iar(is);
        for (In& i_value_with_ws : i_a_value_with_ws)
        {
            iar(i_value_with_ws);
        }
    }

    for (size_t uiIndex = 0; uiIndex < Nb; ++uiIndex)
    {
        CHECK(i_a_value_with_ws[uiIndex] == o_a_value_with_ws[uiIndex]);
    }
}

TEST_CASE("xml_string_issue_consecutive_calls")
{
    std::string strings[] = {
        "some text",
        " some text",
        " some text ",
        "Long text without ws at the end",
        "some text ",
        " some text",
        " some text ",
    };

    test_ws_in_out_array<cereal::XMLInputArchive, cereal::XMLOutputArchive>(strings);
}

TEST_SUITE_END();
