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
#ifndef CEREAL_TEST_BITSET_H_
#define CEREAL_TEST_BITSET_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_bitset()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng32  = [&](){ return random_binary_string<32>( gen ); };
  auto rng65  = [&](){ return random_binary_string<65>( gen ); };
  auto rng256 = [&](){ return random_binary_string<256>( gen ); };
  auto rng512 = [&](){ return random_binary_string<512>( gen ); };

  for(int ii=0; ii<100; ++ii)
  {
    std::bitset<32> o_bit32( rng32() );
    std::bitset<65> o_bit65( rng65() );
    std::bitset<256> o_bit256( rng256() );
    std::bitset<512> o_bit512( rng512() );
    std::bitset<32>  o_bit32_low( 0 );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_bit32);
      oar(o_bit65);
      oar(o_bit256);
      oar(o_bit512);
      oar(o_bit32_low);
    }

    std::bitset<32>  i_bit32;
    std::bitset<65>  i_bit65;
    std::bitset<256> i_bit256;
    std::bitset<512> i_bit512;
    std::bitset<32>  i_bit32_low( 0xffffffff );

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_bit32);
      iar(i_bit65);
      iar(i_bit256);
      iar(i_bit512);
      iar(i_bit32_low);
    }

    CHECK_EQ( o_bit32, i_bit32 );
    CHECK_EQ( o_bit65, i_bit65 );
    CHECK_EQ( o_bit256, i_bit256 );
    CHECK_EQ( o_bit512, i_bit512 );

    CHECK_EQ( o_bit32_low, i_bit32_low );
  }
}

#endif // CEREAL_TEST_BITSET_H_
