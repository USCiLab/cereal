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
#ifndef CEREAL_TEST_UNORDERED_LOADS_H_
#define CEREAL_TEST_UNORDERED_LOADS_H_
#include "common.hpp"

struct unordered_naming
{
  int x;
  int xx;
  int y;
  int z;

  template <class Archive>
  void save( Archive & ar ) const
  {
    ar( CEREAL_NVP(x),
        CEREAL_NVP(z),
        CEREAL_NVP(y),
        CEREAL_NVP(xx) );
  }

  template <class Archive>
  void load( Archive & ar )
  {
    ar( x,
        CEREAL_NVP(y),
        CEREAL_NVP(z),
        CEREAL_NVP(xx) );
  }

  bool operator==( unordered_naming const & other ) const
  {
    return x == other.x && xx == other.xx && y == other.y && z == other.z;
  }
};

std::ostream& operator<<(std::ostream& os, unordered_naming const & s)
{
  os << "[x: " << s.x << " xx: " << s.xx << " y: " << s.y << " z: " << s.z << "]";
  return os;
}

template <class IArchive, class OArchive> inline
void test_unordered_loads()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rngB = [&](){ return random_value<int>( gen ) % 2 == 0; };
  auto rngI = [&](){ return random_value<int>( gen ); };
  auto rngF = [&](){ return random_value<float>( gen ); };
  auto rngD = [&](){ return random_value<double>( gen ); };
  auto rngS = [&](){ return random_basic_string<char>( gen ); };

  for(int ii=0; ii<100; ++ii)
  {
    auto const name1 = rngS();
    auto const name2 = rngS();
    auto const name3 = rngS();
    auto const name4 = rngS();
    auto const name5 = rngS();
    auto const name6 = rngS();
    auto const name7 = rngS();

    int o_int1 = rngI();
    double o_double2 = rngD();
    std::vector<bool> o_vecbool3 = { rngB(), rngB(), rngB(), rngB(), rngB() };
    int o_int4 = rngI();
    int o_int5 = rngI();
    int o_int6 = rngI();
    std::pair<float, unordered_naming> o_un7;
    o_un7.first = rngF();
    o_un7.second.x = rngI();
    o_un7.second.xx = rngI();
    o_un7.second.y = rngI();
    o_un7.second.z = rngI();

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( cereal::make_nvp( name1, o_int1 ),
           cereal::make_nvp( name2, o_double2 ),
           cereal::make_nvp( name3, o_vecbool3 ),
           cereal::make_nvp( name4, o_int4 ),
           cereal::make_nvp( name5, o_int5 ),
           cereal::make_nvp( name6, o_int6 ),
           cereal::make_nvp( name7, o_un7 ) );
    }

    decltype(o_int1) i_int1;
    decltype(o_double2) i_double2;
    decltype(o_vecbool3) i_vecbool3;
    decltype(o_int4) i_int4;
    decltype(o_int5) i_int5;
    decltype(o_int6) i_int6;
    decltype(o_un7) i_un7;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( cereal::make_nvp( name7, i_un7 ),
           cereal::make_nvp( name2, i_double2 ),
           cereal::make_nvp( name4, i_int4 ),
           cereal::make_nvp( name3, i_vecbool3 ),
           cereal::make_nvp( name1, i_int1 ),
           cereal::make_nvp( name5, i_int5 ),
           i_int6 );
    }

    CHECK_EQ(o_int1, i_int1);
    CHECK_EQ(o_double2, doctest::Approx(o_double2).epsilon(1e-5));
    CHECK_EQ(o_vecbool3.size(), i_vecbool3.size());
    check_collection(i_vecbool3, o_vecbool3);
    CHECK_EQ(o_int4, i_int4);
    CHECK_EQ(o_int5, i_int5);
    CHECK_EQ(o_int6, i_int6);
    CHECK_EQ(o_un7.first, i_un7.first);
    CHECK_EQ(o_un7.second, i_un7.second);
  }
}

#endif // CEREAL_TEST_UNORDERED_LOADS_H_
