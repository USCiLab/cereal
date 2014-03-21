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

struct OneLA
{
  OneLA( int xx ) : x( xx ) {}

  int x;

  template <class Archive>
  void serialize( Archive & ar )
  { ar( x ); }

  template <class Archive>
  static void load_and_construct( Archive & ar, cereal::construct<OneLA> & construct )
  {
    int xx;
    ar( xx );
    construct( xx );
  }

  bool operator==( OneLA const & other ) const
  { return x == other.x; }
};

std::ostream& operator<<(std::ostream& os, OneLA const & s)
{
  os << "[" << s.x << "]";
  return os;
}

struct TwoLA
{
  TwoLA( int xx ) : x( xx ) {}

  int x;

  template <class Archive>
  void serialize( Archive & ar )
  { ar( x ); }

  bool operator==( TwoLA const & other ) const
  { return x == other.x; }
};

std::ostream& operator<<(std::ostream& os, TwoLA const & s)
{
  os << "[" << s.x << "]";
  return os;
}

namespace cereal
{
  template <>
  struct LoadAndConstruct<TwoLA>
  {
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<TwoLA> & construct )
    {
      int xx;
      ar( xx );
      construct( xx );
    }
  };
}

struct ThreeLA : std::enable_shared_from_this<ThreeLA>
{
  ThreeLA( int xx ) : x( xx ) {}

  int x;

  template <class Archive>
  void serialize( Archive & ar )
  { ar( x ); }

  bool operator==( ThreeLA const & other ) const
  { return x == other.x; }

  template <class Archive>
  static void load_and_construct( Archive & ar, cereal::construct<ThreeLA> & construct )
  {
    int xx;
    ar( xx );
    construct( xx );
  }
};

std::ostream& operator<<(std::ostream& os, ThreeLA const & s)
{
  os << "[" << s.x << "]";
  return os;
}

template <class IArchive, class OArchive>
void test_memory_load_construct()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    auto o_shared1 = std::make_shared<OneLA>( random_value<int>(gen) );
    auto o_shared2 = std::make_shared<TwoLA>( random_value<int>(gen) );
    std::unique_ptr<OneLA> o_unique1( new OneLA( random_value<int>(gen) ) );
    std::unique_ptr<TwoLA> o_unique2( new TwoLA( random_value<int>(gen) ) );
    auto o_shared3 = std::make_shared<ThreeLA>( random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_shared1 );
      oar( o_shared2 );
      oar( o_unique1 );
      oar( o_unique2 );
      oar( o_shared3 );
    }

    o_shared3->shared_from_this(); // tests github issue #68

    decltype(o_shared1) i_shared1;
    decltype(o_shared2) i_shared2;
    decltype(o_unique1) i_unique1;
    decltype(o_unique2) i_unique2;
    decltype(o_shared3) i_shared3;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_shared1 );
      iar( i_shared2 );
      iar( i_unique1 );
      iar( i_unique2 );
      iar( i_shared3 );
    }

    BOOST_CHECK_EQUAL( *o_shared1, *i_shared1 );
    BOOST_CHECK_EQUAL( *o_shared2, *i_shared2 );
    BOOST_CHECK_EQUAL( *o_unique1, *i_unique1 );
    BOOST_CHECK_EQUAL( *o_unique2, *i_unique2 );
    BOOST_CHECK_EQUAL( *o_shared3, *i_shared3 );

    auto i_shared3_2 = i_shared3->shared_from_this();
    BOOST_CHECK_EQUAL( *o_shared3, *i_shared3_2 );
  }
}

BOOST_AUTO_TEST_CASE( binary_memory_load_construct )
{
  test_memory_load_construct<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_memory_load_construct )
{
  test_memory_load_construct<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_memory_load_construct )
{
  test_memory_load_construct<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_memory_load_construct )
{
  test_memory_load_construct<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

