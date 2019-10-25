
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

#define MAKE_TEST_HELPER_FUNCS(t)                             \
bool operator==(t const & l, t const & r)                     \
{ return l.x == r.x; }                                        \
inline std::ostream& operator<<(std::ostream&os, t const & s) \
{                                                             \
    os << "[x: " << s.x << "]";                               \
    return os;                                                \
}

struct TrivialISER
{
  int x;

  template <class Archive>
  void serialize( Archive & ar )
  { ar( x ); }
};

MAKE_TEST_HELPER_FUNCS(TrivialISER)

struct TrivialISPL
{
  int x;

  template <class Archive>
  void load( Archive & ar )
  { ar( x ); }

  template <class Archive>
  void save( Archive & ar ) const
  { ar( x ); }
};

MAKE_TEST_HELPER_FUNCS(TrivialISPL)

struct TrivialESER
{
  int x;
};

template <class Archive>
void serialize( Archive & ar, TrivialESER & t )
{ ar( t.x ); }

MAKE_TEST_HELPER_FUNCS(TrivialESER)

struct TrivialESPL
{
  int x;
};

template <class Archive>
void load( Archive & ar, TrivialESPL & t )
{ ar( t.x ); }

template <class Archive>
void save( Archive & ar, TrivialESPL const & t )
{ ar( t.x ); }

MAKE_TEST_HELPER_FUNCS(TrivialESPL)
#undef MAKE_TEST_HELPER_FUNCS

template <class IArchive, class OArchive>
void test_atomic()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::atomic<bool> o_ab(random_value<uint8_t>(gen) % 2 ? true : false);
    std::atomic<signed char> o_asc(random_value<signed char>(gen));
    std::atomic<unsigned short> o_aus(random_value<unsigned short>(gen));
    std::atomic<int> o_asi(random_value<int>(gen));
    std::atomic<long> o_asl(random_value<long>(gen));
    std::atomic<unsigned long long> o_aull(random_value<unsigned long long>(gen));
    std::atomic<double> o_ad(random_value<double>(gen));
    std::atomic<TrivialISER> o_iser{{random_value<int>(gen)}};
    std::atomic<TrivialISPL> o_ispl{{random_value<int>(gen)}};
    std::atomic<TrivialESER> o_eser{{random_value<int>(gen)}};
    std::atomic<TrivialESPL> o_espl{{random_value<int>(gen)}};

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_ab);
      oar(o_asc);
      oar(o_aus);
      oar(o_asi);
      oar(o_asl);
      oar(o_aull);
      oar(o_ad);
      oar(o_iser);
      oar(o_ispl);
      oar(o_eser);
      oar(o_espl);
    }

    decltype(o_ab) i_ab;
    decltype(o_asc) i_asc;
    decltype(o_aus) i_aus;
    decltype(o_asi) i_asi;
    decltype(o_asl) i_asl;
    decltype(o_aull) i_aull;
    decltype(o_ad) i_ad;
    decltype(o_iser) i_iser;
    decltype(o_ispl) i_ispl;
    decltype(o_eser) i_eser;
    decltype(o_espl) i_espl;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_ab);
      iar(i_asc);
      iar(i_aus);
      iar(i_asi);
      iar(i_asl);
      iar(i_aull);
      iar(i_ad);
      iar(i_iser);
      iar(i_ispl);
      iar(i_eser);
      iar(i_espl);
    }

    CHECK_EQ(o_ab.load(), i_ab.load());
    CHECK_EQ(o_asc.load(), i_asc.load());
    CHECK_EQ(o_aus.load(), i_aus.load());
    CHECK_EQ(o_asi.load(), i_asi.load());
    CHECK_EQ(o_asl.load(), i_asl.load());
    CHECK_EQ(o_aull.load(), i_aull.load());
    CHECK_EQ(o_ad.load(), doctest::Approx(i_ad.load()).epsilon(1e-5L));
    CHECK_EQ(i_iser.load(), o_iser.load());
    CHECK_EQ(i_ispl.load(), o_ispl.load());
    CHECK_EQ(i_eser.load(), o_eser.load());
    CHECK_EQ(i_espl.load(), o_espl.load());
  }
}
