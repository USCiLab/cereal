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
void test_bitset()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng32  = [&](){ return random_binary_string<32>( gen ); };
  auto rng65  = [&](){ return random_binary_string<65>( gen ); };
  auto rng256 = [&](){ return random_binary_string<256>( gen ); };

  for(int ii=0; ii<100; ++ii)
  {
    std::bitset<32> o_bit32( rng32() );
    std::bitset<65> o_bit65( rng65() );
    std::bitset<256> o_bit256( rng256() );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_bit32);
      oar(o_bit65);
      oar(o_bit256);
    }

    std::bitset<32>  i_bit32;
    std::bitset<65>  i_bit65;
    std::bitset<256> i_bit256;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_bit32);
      iar(i_bit65);
      iar(i_bit256);
    }

    BOOST_CHECK_EQUAL( o_bit32, i_bit32 );
    BOOST_CHECK_EQUAL( o_bit65, i_bit65 );
    BOOST_CHECK_EQUAL( o_bit256, i_bit256 );
  }
}

BOOST_AUTO_TEST_CASE( binary_bitset )
{
  test_bitset<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_bitset )
{
  test_bitset<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_bitset )
{
  test_bitset<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_bitset )
{
  test_bitset<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


