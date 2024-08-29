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
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TEST_COMPLEX_H_
#define CEREAL_TEST_COMPLEX_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
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

    CHECK_EQ( o_float, i_float );
    CHECK_EQ( o_double.real(), doctest::Approx(i_double.real()).epsilon(1e-5) );
    CHECK_EQ( o_double.imag(), doctest::Approx(i_double.imag()).epsilon(1e-5) );
    CHECK_EQ( o_ldouble.real(), doctest::Approx(i_ldouble.real()).epsilon(1e-5L) );
    CHECK_EQ( o_ldouble.imag(), doctest::Approx(i_ldouble.imag()).epsilon(1e-5L) );
  }
}

#endif // CEREAL_TEST_COMPLEX_H_
