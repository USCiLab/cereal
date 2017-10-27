/*
  Copyright (c) 2017, Juan Pedro Bolivar Puente
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

#include "common.hpp"

#if __cplusplus >= 201703L

#include <cereal/types/optional.hpp>

template <class IArchive, class OArchive> inline
void test_std_optional()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  std::optional<int> o_bv1 = random_value<int>(gen);
  std::optional<double> o_bv2 = random_value<double>(gen);
  std::optional<std::string> o_bv3 = random_basic_string<char>(gen);
  std::optional<int> o_bv4 = std::nullopt;
  std::optional<double> o_bv5 = std::nullopt;
  std::optional<std::string> o_bv6 = std::nullopt;

  std::ostringstream os;
  {
    OArchive oar(os);

    oar(o_bv1);
    oar(o_bv2);
    oar(o_bv3);
    oar(o_bv4);
    oar(o_bv5);
    oar(o_bv6);
  }

  decltype(o_bv1) i_bv1;
  decltype(o_bv2) i_bv2;
  decltype(o_bv3) i_bv3;
  decltype(o_bv4) i_bv4;
  decltype(o_bv5) i_bv5;
  decltype(o_bv6) i_bv6;

  std::istringstream is(os.str());
  {
    IArchive iar(is);

    iar(i_bv1);
    iar(i_bv2);
    iar(i_bv3);
  }

  CHECK_EQ( *i_bv1, std::get<int>(o_bv1) );
  CHECK_EQ( *i_bv2, doctest::Approx(std::get<double>(o_bv2)).epsilon(1e-5) );
  CHECK_EQ( *i_bv3, std::get<std::string>(o_bv3) );
  CHECK( !i_bv4 );
  CHECK( !i_bv5 );
  CHECK( !i_bv6 );
}

TEST_SUITE("std_optional");

TEST_CASE("binary_std_optional")
{
  test_std_optional<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

TEST_CASE("portable_binary_std_optional")
{
  test_std_optional<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

TEST_CASE("xml_std_optional")
{
  test_std_optional<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

TEST_CASE("json_std_optional")
{
  test_std_optional<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

TEST_SUITE_END();

#endif // is c++17
