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
void test_pod()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    bool     const o_bool   = random_value<uint8_t>(gen) % 2 ? true : false;
    uint8_t  const o_uint8  = random_value<uint8_t>(gen);
    int8_t   const o_int8   = random_value<int8_t>(gen);
    uint16_t const o_uint16 = random_value<uint16_t>(gen);
    int16_t  const o_int16  = random_value<int16_t>(gen);
    uint32_t const o_uint32 = random_value<uint32_t>(gen);
    int32_t  const o_int32  = random_value<int32_t>(gen);
    uint64_t const o_uint64 = random_value<uint64_t>(gen);
    int64_t  const o_int64  = random_value<int64_t>(gen);
    float    const o_float  = random_value<float>(gen);
    double   const o_double = random_value<double>(gen);

    long double const o_long_double = random_value<long double>(gen);
    long const o_long = random_value<long>(gen);
    unsigned long const o_ulong = random_value<unsigned long>(gen);
    long long const o_long_long = random_value<long long>(gen);
    unsigned long long const o_ulong_long = random_value<unsigned long long>(gen);

    std::ostringstream os;
    {
      OArchive oar(os);
      oar(o_bool);
      oar(o_uint8);
      oar(o_int8);
      oar(o_uint16);
      oar(o_int16);
      oar(o_uint32);
      oar(o_int32);
      oar(o_uint64);
      oar(o_int64);
      oar(o_float);
      oar(o_double);
      oar(o_long_double);
      oar(o_long);
      oar(o_ulong);
      oar(o_long_long);
      oar(o_ulong_long);
    }

    bool     i_bool                       = false;
    uint8_t  i_uint8                      = 0;
    int8_t   i_int8                       = 0;
    uint16_t i_uint16                     = 0;
    int16_t  i_int16                      = 0;
    uint32_t i_uint32                     = 0;
    int32_t  i_int32                      = 0;
    uint64_t i_uint64                     = 0;
    int64_t  i_int64                      = 0;
    float    i_float                      = 0;
    double   i_double                     = 0;

    long double i_long_double       = 0;
    long i_long                     = 0;
    unsigned long i_ulong           = 0;
    long long i_long_long           = 0;
    unsigned long long i_ulong_long = 0;

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      iar(i_bool);
      iar(i_uint8);
      iar(i_int8);
      iar(i_uint16);
      iar(i_int16);
      iar(i_uint32);
      iar(i_int32);
      iar(i_uint64);
      iar(i_int64);
      iar(i_float);
      iar(i_double);
      iar(i_long_double);
      iar(i_long);
      iar(i_ulong);
      iar(i_long_long);
      iar(i_ulong_long);
    }

    BOOST_CHECK_EQUAL(i_bool   , o_bool);
    BOOST_CHECK_EQUAL(i_uint8  , o_uint8);
    BOOST_CHECK_EQUAL(i_int8   , o_int8);
    BOOST_CHECK_EQUAL(i_uint16 , o_uint16);
    BOOST_CHECK_EQUAL(i_int16  , o_int16);
    BOOST_CHECK_EQUAL(i_uint32 , o_uint32);
    BOOST_CHECK_EQUAL(i_int32  , o_int32);
    BOOST_CHECK_EQUAL(i_uint64 , o_uint64);
    BOOST_CHECK_EQUAL(i_int64  , o_int64);
    BOOST_CHECK_CLOSE(i_float  , o_float,  (float)1e-5);
    BOOST_CHECK_CLOSE(i_double , o_double, 1e-5);

    BOOST_CHECK_CLOSE(i_long_double, o_long_double, 1e-5);
    BOOST_CHECK_EQUAL(i_long,        o_long);
    BOOST_CHECK_EQUAL(i_ulong,       o_ulong);
    BOOST_CHECK_EQUAL(i_long_long,   o_long_long);
    BOOST_CHECK_EQUAL(i_ulong_long,  o_ulong_long);
  }
}

BOOST_AUTO_TEST_CASE( binary_pod )
{
  test_pod<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_pod )
{
  test_pod<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_pod )
{
  test_pod<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_pod )
{
  test_pod<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
