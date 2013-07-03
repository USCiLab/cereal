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
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

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

    int x;
};

class Derived : public Base
{
  public:
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

    int y;
};

namespace cereal
{
  template <class Archive> struct specialize<Archive, Derived, cereal::specialization::member_load_save> {};
  //template <class Archive> struct specialize<Archive, Derived, cereal::specialization::non_member_load_save> {};
}

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
  void serialize(Archive & ar)
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
  void serialize( Archive & archive )
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
    static NoDefaultCtor * load_and_allocate( Archive & ar )
    {
      return new NoDefaultCtor(5);
    }
  };
}

// ######################################################################
int main()
{
  std::cout << std::boolalpha << std::endl;

  std::stringstream os;
  cereal::BinaryOutputArchive archive(os);

  Derived d;
  archive( d );

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
    //std::stringstream os;
    std::ofstream os("out.xml");
    cereal::XMLOutputArchive oar( os );
    oar( cereal::make_nvp("hello", 5 ) );
    std::string bla("bla");
    oar( bla );

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

    Everything e;
    oar( cereal::make_nvp("EVERYTHING", e) );
    oar( vec );
    oar( vec2 );

    int xxx[] = {-1, 95, 3};
    oar.saveBinaryValue( xxx, sizeof(int)*3, "xxxbinary" );
    oar.saveBinaryValue( xxx, sizeof(int)*3 );
  }

  {
    std::ifstream is("out.xml");
    cereal::XMLInputArchive iar( is );

    //int z;
    //iar( cereal::make_nvp("hello", z) );
  }




  return 0;
}
