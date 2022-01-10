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
#ifndef CEREAL_TEST_LOAD_CONSTRUCT_H_
#define CEREAL_TEST_LOAD_CONSTRUCT_H_
#include "common.hpp"

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

struct OneLAVersioned
{
  OneLAVersioned( int xx ) : x( xx ), v() {}
  OneLAVersioned( int xx, int vv ) : x( xx ), v( vv ) {}

  int x;
  std::uint32_t v;

  template <class Archive>
  void serialize( Archive & ar, const std::uint32_t version )
  { ar( x ); v = version; }

  template <class Archive>
  static void load_and_construct( Archive & ar, cereal::construct<OneLAVersioned> & construct, const std::uint32_t version )
  {
    int xx;
    ar( xx );
    construct( xx, version );
  }

  bool operator==( OneLAVersioned const & other ) const
  { return x == other.x; }
};

std::ostream& operator<<(std::ostream& os, OneLAVersioned const & s)
{
  os << "[" << s.x << "]";
  return os;
}

CEREAL_CLASS_VERSION( OneLAVersioned, 13 )

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

struct TwoLAVersioned
{
  TwoLAVersioned( int xx ) : x( xx ), v() {}
  TwoLAVersioned( int xx, int vv ) : x( xx ), v( vv ) {}

  int x;
  std::uint32_t v;

  template <class Archive>
  void serialize( Archive & ar, const std::uint32_t version )
  { ar( x ); v = version; }

  bool operator==( TwoLAVersioned const & other ) const
  { return x == other.x; }
};

std::ostream& operator<<(std::ostream& os, TwoLAVersioned const & s)
{
  os << "[" << s.x << "]";
  return os;
}

namespace cereal
{
  template <>
  struct LoadAndConstruct<TwoLAVersioned>
  {
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<TwoLAVersioned> & construct, const std::uint32_t version )
    {
      int xx;
      ar( xx );
      construct( xx, version );
    }
  };
}

CEREAL_CLASS_VERSION( TwoLAVersioned, 1 )

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
    auto o_shared1v = std::make_shared<OneLAVersioned>( random_value<int>(gen) );
    auto o_shared2v = std::make_shared<TwoLAVersioned>( random_value<int>(gen) );

    auto o_constShared1 = std::make_shared<const OneLA>( random_value<int>(gen) ); // issue 417
    auto o_constShared2 = std::make_shared<const TwoLA>( random_value<int>(gen) );
    std::unique_ptr<const OneLA> o_constUnique1( new OneLA( random_value<int>(gen) ) );
    std::unique_ptr<const TwoLA> o_constUnique2( new TwoLA( random_value<int>(gen) ) );
    auto o_constShared3 = std::make_shared<const ThreeLA>( random_value<int>(gen) );
    auto o_constShared1v = std::make_shared<const OneLAVersioned>( random_value<int>(gen) );
    auto o_constShared2v = std::make_shared<const TwoLAVersioned>( random_value<int>(gen) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_shared1 );
      oar( o_shared2 );
      oar( o_unique1 );
      oar( o_unique2 );
      oar( o_shared3 );
      oar( o_shared1v );
      oar( o_shared2v );
      oar( o_constShared1 );
      oar( o_constShared2 );
      oar( o_constUnique1 );
      oar( o_constUnique2 );
      oar( o_constShared3 );
      oar( o_constShared1v );
      oar( o_constShared2v );
    }

    (void) o_shared3->shared_from_this(); // tests github issue #68
    (void) o_constShared3->shared_from_this();

    decltype(o_shared1) i_shared1;
    decltype(o_shared2) i_shared2;
    decltype(o_unique1) i_unique1;
    decltype(o_unique2) i_unique2;
    decltype(o_shared3) i_shared3;
    decltype(o_shared1v) i_shared1v;
    decltype(o_shared2v) i_shared2v;
    decltype(o_constShared1) i_constShared1;
    decltype(o_constShared2) i_constShared2;
    decltype(o_constUnique1) i_constUnique1;
    decltype(o_constUnique2) i_constUnique2;
    decltype(o_constShared3) i_constShared3;
    decltype(o_constShared1v) i_constShared1v;
    decltype(o_constShared2v) i_constShared2v;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_shared1 );
      iar( i_shared2 );
      iar( i_unique1 );
      iar( i_unique2 );
      iar( i_shared3 );
      iar( i_shared1v );
      iar( i_shared2v );
      iar( i_constShared1 );
      iar( i_constShared2 );
      iar( i_constUnique1 );
      iar( i_constUnique2 );
      iar( i_constShared3 );
      iar( i_constShared1v );
      iar( i_constShared2v );
    }

    CHECK_EQ( *o_shared1, *i_shared1 );
    CHECK_EQ( *o_shared2, *i_shared2 );
    CHECK_EQ( *o_unique1, *i_unique1 );
    CHECK_EQ( *o_unique2, *i_unique2 );
    CHECK_EQ( *o_shared3, *i_shared3 );
    CHECK_EQ( *o_shared1v, *i_shared1v );
    CHECK_EQ(i_shared1v->v, 13u);
    CHECK_EQ( *o_shared2v, *i_shared2v );
    CHECK_EQ(i_shared2v->v, 1u);

    auto i_shared3_2 = i_shared3->shared_from_this();
    CHECK_EQ( *o_shared3, *i_shared3_2 );

    CHECK_EQ( *o_constShared1, *i_constShared1 );
    CHECK_EQ( *o_constShared2, *i_constShared2 );
    CHECK_EQ( *o_constUnique1, *i_constUnique1 );
    CHECK_EQ( *o_constUnique2, *i_constUnique2 );
    CHECK_EQ( *o_constShared3, *i_constShared3 );
    CHECK_EQ( *o_constShared1v, *i_constShared1v );
    CHECK_EQ(i_constShared1v->v, 13u);
    CHECK_EQ( *o_constShared2v, *i_constShared2v );
    CHECK_EQ(i_constShared2v->v, 1u);

    auto i_constShared3_2 = i_constShared3->shared_from_this();
    CHECK_EQ( *o_constShared3, *i_constShared3_2 );
  }
}

#endif // CEREAL_TEST_LOAD_CONSTRUCT_H_
