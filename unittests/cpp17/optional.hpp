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
#ifndef CEREAL_TEST_CPP17_OPTIONAL_H_
#define CEREAL_TEST_CPP17_OPTIONAL_H_
#include "../common.hpp"

#ifdef CEREAL_HAS_CPP17
#include <cereal/types/optional.hpp>

template <class IArchive, class OArchive> inline
void test_std_optional()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  std::optional<int> o_o1 = random_value<int>(gen);
  std::optional<double> o_o2 = random_value<double>(gen);
  std::optional<std::string> o_o3 = random_basic_string<char>(gen);
  std::optional<int> o_o4 = std::nullopt;
  std::optional<double> o_o5 = std::nullopt;
  std::optional<std::string> o_o6 = std::nullopt;
  std::optional<std::optional<long>> o_o7 = std::make_optional<std::optional<long>>( std::make_optional<long>( random_value<long>(gen) ) );

  std::ostringstream os;
  {
    OArchive oar(os);

    oar(o_o1);
    oar(o_o2);
    oar(o_o3);
    oar(o_o4);
    oar(o_o5);
    oar(o_o6);
    oar(o_o7);
  }

  decltype(o_o1) i_o1;
  decltype(o_o2) i_o2;
  decltype(o_o3) i_o3;
  decltype(o_o4) i_o4{1}; // initialize with non-nullopt
  decltype(o_o5) i_o5{1.0};
  decltype(o_o6) i_o6{"1"};
  decltype(o_o7) i_o7;

  std::istringstream is(os.str());
  {
    IArchive iar(is);

    iar(i_o1);
    iar(i_o2);
    iar(i_o3);
    iar(i_o4);
    iar(i_o5);
    iar(i_o6);
    iar(i_o7);
  }

  CHECK_EQ( *i_o1, *o_o1 );
  CHECK_EQ( *i_o2, doctest::Approx(*o_o2).epsilon(1e-5) );
  CHECK_EQ( *i_o3, *o_o3 );
  CHECK_EQ( i_o4.has_value(), o_o4.has_value() );
  CHECK_EQ( i_o5.has_value(), o_o5.has_value() );
  CHECK_EQ( i_o6.has_value(), o_o6.has_value() );
  CHECK_EQ( **i_o7, **o_o7 );
}

#endif // CEREAL_HAS_CPP17
#endif // CEREAL_TEST_CPP17_OPTIONAL_H_
