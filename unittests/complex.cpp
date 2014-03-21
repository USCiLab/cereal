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
void test_complex()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rngF = [&](){ return random_value<float>(gen); };
  auto rngD = [&](){ return random_value<double>(gen); };
  auto rngLD = [&](){ return random_value<long double>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    std::complex<float> o_float( rngF(), rngF() );
    std::complex<double> o_double( rngD(), rngD() );
    std::complex<long double> o_ldouble( rngLD(), rngLD() );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_float);
      oar(o_double);
      oar(o_ldouble);
    }

    std::complex<float> i_float;
    std::complex<double> i_double;
    std::complex<long double> i_ldouble;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_float);
      iar(i_double);
      iar(i_ldouble);
    }

    BOOST_CHECK_EQUAL( o_float, i_float );
    BOOST_CHECK_CLOSE( o_double.real(), i_double.real(), 1e-5);
    BOOST_CHECK_CLOSE( o_double.imag(), i_double.imag(), 1e-5);
    BOOST_CHECK_CLOSE( o_ldouble.real(), i_ldouble.real(), 1e-5);
    BOOST_CHECK_CLOSE( o_ldouble.imag(), i_ldouble.imag(), 1e-5);
  }
}

BOOST_AUTO_TEST_CASE( binary_complex )
{
  test_complex<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_complex )
{
  test_complex<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_complex )
{
  test_complex<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_complex )
{
  test_complex<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


