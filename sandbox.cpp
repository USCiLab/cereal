/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
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

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/polymorphic.hpp>

#include <cxxabi.h>
#include <sstream>
#include <fstream>
#include <cassert>
#include <complex>
#include <iostream>

class Base
{
  private:
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
      std::cout << "Base serialize" << std::endl;
      ar( x );
    }

    virtual void foo() = 0;

  public:
    int x;
};

class Derived : public Base
{
  public:
    using Base::x;
    Derived() = default;
    Derived( int d, int b )
    {
      y = d;
      x = b;
    }

    template <class Archive>
    void save( Archive & ar ) const
    {
      ar( cereal::virtual_base_class<Base>(this) );
      std::cout << "Derived save" << std::endl;
      ar( y );
    }

    template <class Archive>
    void load( Archive & ar )
    {
      ar( cereal::virtual_base_class<Base>(this) );
      std::cout << "Derived load" << std::endl;
      ar( y );
    }

    void foo() {}

    int y;
};

namespace cereal
{
  template <class Archive> struct specialize<Archive, Derived, cereal::specialization::member_load_save> {};
}

CEREAL_REGISTER_TYPE(Derived);

// ###################################
struct Test1
{
  int a;

  private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive & ar)
    {
      ar(CEREAL_NVP(a));
    }
};

// ###################################
class Test2
{
  public:
    Test2() {}
    Test2( int x ) : a( x ) {}
    int a;

  private:
    friend class cereal::access;

    template<class Archive>
      void save(Archive & ar) const
      {
        ar(a);
      }

    template<class Archive>
      void load(Archive & ar)
      {
        ar(a);
      }
};

// ###################################
struct Test3
{
  int a;
};

template<class Archive>
void serialize(Archive & ar, Test3 & t)
{
  ar(CEREAL_NVP(t.a));
}

namespace test4
{
  // ###################################
  struct Test4
  {
    int a;
  };

  template<class Archive>
  void save(Archive & ar, Test4 const & t)
  {
    ar(CEREAL_NVP(t.a));
  }

  template<class Archive>
  void load(Archive & ar, Test4 & t)
  {
    ar(CEREAL_NVP(t.a));
  }
}

class Private
{
  public:
    Private() : a('z') {}

  private:
    char a;

    friend class cereal::access;

    template<class Archive>
      void serialize(Archive & ar)
      {
        ar(a);
      }
};

struct Everything
{
  int x;
  int y;
  Test1 t1;
  Test2 t2;
  Test3 t3;
  test4::Test4 t4;
  std::string s;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar(CEREAL_NVP(x));
    ar(CEREAL_NVP(y));
    ar(CEREAL_NVP(t1));
    ar(CEREAL_NVP(t2));
    ar(CEREAL_NVP(t3));
    ar(CEREAL_NVP(t4));
    ar(CEREAL_NVP(s));
  }

  bool operator==(Everything const & o)
  {
    return
      x == o.x &&
      y == o.y &&
      t1.a == o.t1.a &&
      t2.a == o.t2.a &&
      t3.a == o.t3.a &&
      t4.a == o.t4.a &&
      s == o.s;
  }
};

struct EmptyStruct
{
  template<class Archive>
  void serialize(Archive &)
  {
    std::cout << "Side effects!" << std::endl;
  };
};

struct NonEmptyStruct
{
  int x, y, z;
};

struct NoDefaultCtor
{
  NoDefaultCtor() = delete;
  NoDefaultCtor(int x) : y(x)
  { }

  int y;

  template <class Archive>
  void serialize( Archive & )
  {
  }

  //template <class Archive>
  //static NoDefaultCtor * load_and_allocate( Archive & ar )
  //{
  //  return new NoDefaultCtor(5);
  //}
};

namespace cereal
{
  template <>
  struct LoadAndAllocate<NoDefaultCtor>
  {
    template <class Archive>
    static NoDefaultCtor * load_and_allocate( Archive & )
    {
      return new NoDefaultCtor(5);
    }
  };
}

// ######################################################################
int main()
{
  std::cout << std::boolalpha << std::endl;

  Everything e_out;
  e_out.x = 99;
  e_out.y = 100;
  e_out.t1 = {1};
  e_out.t2 = {2};
  e_out.t3 = {3};
  e_out.t4 = {4};
  e_out.s = "Hello, World!";

  Test2 t2 = {22};

  {
    std::ofstream os("out.txt");
    cereal::BinaryOutputArchive archive(os);
    archive(CEREAL_NVP(e_out));
    archive(t2);
  }

  Everything e_in;

  {
    std::ifstream is("out.txt");
    cereal::BinaryInputArchive archive(is);
    archive(CEREAL_NVP(e_in));
    archive(t2);
    std::remove("out.txt");
  }

  assert(e_in == e_out);

  {
    cereal::BinaryOutputArchive archive(std::cout);
    int xxx[] = {-1, 95, 3};
    archive( xxx );

    cereal::XMLOutputArchive archive2(std::cout, 10);
    archive2( xxx );

    std::vector<int> yyy = {1, 2, 3};
    archive2( yyy );

    archive2.saveBinaryValue( xxx, sizeof(int)*3 );
  }

  {
    std::ofstream os("out.xml");
    cereal::XMLOutputArchive oar( os );
    //cereal::XMLOutputArchive oar( std::cout );

    oar( cereal::make_nvp("hello", 5 ) );

    std::string bla("bla");
    oar( bla );

    auto intptr = std::make_shared<int>(99);
    oar( CEREAL_NVP(intptr) );

    std::map<std::string, int> map1 =
    {
      {"one",   1},
      {"two",   2},
      {"three", 3}
    };

    oar( CEREAL_NVP(map1) );

    int x = 3;
    oar( CEREAL_NVP(x) );
    oar( 5 );
    oar( 3.3 );
    oar( 3.2f );
    oar( true );

    std::array<int,5> arr = {{1, 2, 3, 4, 5}};
    oar( arr );

    std::vector<std::string> vec = {"hey",
                                    "there",
                                    "buddy"};

    std::vector<std::vector<std::string>> vec2 = {vec, vec, vec};

    oar( cereal::make_nvp("EVERYTHING", e_out) );
    oar( vec );
    oar( vec2 );

    int xxx[] = {-1, 95, 3};
    oar.saveBinaryValue( xxx, sizeof(int)*3, "xxxbinary" );
    //oar.saveBinaryValue( xxx, sizeof(int)*3 );

    std::unique_ptr<Derived> d1( new Derived(3, 4) );
    std::unique_ptr<Base> d2( new Derived(4, 5) );
    std::shared_ptr<Base> d3( new Derived(5, 6) );
    oar( d1 );
    oar( d2 );
    oar( d3 );
  }

  {
    std::ifstream is("out.xml");
    cereal::XMLInputArchive iar( is );

    int hello;
    iar( cereal::make_nvp("hello", hello) );
    assert( hello == 5 );

    std::string bla;
    iar( bla );
    assert( bla == "bla" );

    std::shared_ptr<int> intptr;
    iar( CEREAL_NVP(intptr) );
    assert( *intptr == 99 );

    std::map<std::string, int> map1;

    iar( CEREAL_NVP(map1) );
    assert( map1["one"]   == 1 );
    assert( map1["two"]   == 2 );
    assert( map1["three"] == 3 );


    int x;
    iar( CEREAL_NVP(x) );
    assert( x == 3 );

    int x5;
    iar( x5 );
    assert( x5 == 5 );

    double x33;
    iar( x33 );
    assert( x33 == 3.3 );

    float x32;
    iar( x32 );
    assert( x32 == 3.2f );

    bool xtrue;
    iar( xtrue );
    assert( xtrue == true );

    std::array<int,5> arr;
    iar( arr );
    for( int i = 0; i < 5; ++i )
      assert( arr[i] == (i+1) );

    Everything e;
    iar( cereal::make_nvp("EVERYTHING", e) );
    assert( e == e_out );

    std::vector<std::string> vec;
    iar( vec );
    assert( vec[0] == "hey" );
    assert( vec[1] == "there" );
    assert( vec[2] == "buddy" );

    std::vector<std::vector<std::string>> vec2;
    iar( vec2 );
    for( auto & v : vec2 )
    {
      assert( v[0] == "hey" );
      assert( v[1] == "there" );
      assert( v[2] == "buddy" );
    }

    int xxx[3];
    iar.loadBinaryValue( xxx, sizeof(int)*3 );
    assert( xxx[0] == -1 );
    assert( xxx[1] == 95 );
    assert( xxx[2] == 3 );

    std::unique_ptr<Derived> d1;
    std::unique_ptr<Base> d2;
    std::shared_ptr<Base> d3;

    iar( d1 );
    assert( d1->x == 4 && d1->y == 3 );
    iar( d2 );
    assert( ((Derived*)d2.get())->x == 5 && ((Derived*)d2.get())->y == 4 );
    iar( d3 );
    assert( ((Derived*)d3.get())->x == 6 && ((Derived*)d3.get())->y == 5 );
  }

  {
    std::ofstream b("endian.out");
    cereal::PortableBinaryOutputArchive oar(b);

    bool bb = true;
    char a = 'a';
    int x = 1234;
    float y = 1.324f;
    double z = 3.1452;
    long double d = 1.123451234512345;
    long long j = 2394873298472343;

    oar( bb, a, x, y, z, d, j );
    std::cout << bb << " " << a << " " << x << " " << y << " " << z << " " << d << " " << j << std::endl;
  }
  {
    std::ifstream b("endian.out");
    cereal::PortableBinaryInputArchive iar(b);

    bool bb;
    char a;
    int x;
    float y;
    double z;
    long double d;
    long long j;

    iar( bb, a, x, y, z, d, j );

    std::cout << bb << " " << a << " " << x << " " << y << " " << z << " " << d << " " << j << std::endl;

    std::remove("endian.out");
  }



  return 0;
}
