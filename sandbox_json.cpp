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
#include <cereal/archives/json.hpp>

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

struct SubFixture
{
    int a      = 3;
    uint64_t b = 9999;
    float c    = 100.1;
    double d   = 2000.9;
    std::string s = "hello, world!";

    template<class Archive>
      void serialize(Archive & ar)
      {
        ar( CEREAL_NVP(a),
            b,
            c,
            CEREAL_NVP(d),
            CEREAL_NVP(s) );
      }
};

struct Fixture
{
  SubFixture f1, f2, f3;
    template<class Archive>
      void serialize(Archive & ar)
      {
        ar( f1,
            CEREAL_NVP(f2),
            f3 );
      }

};

void foo(int t)
{
  std::cout << "int" << std::endl;
}

void foo(unsigned int t)
{
  std::cout << "unsigned int" << std::endl;
}

// ######################################################################
int main()
{
  std::cout << std::boolalpha << std::endl;

  {
    cereal::JSONOutputArchive oar( std::cout );

    Fixture fixture;

    oar( CEREAL_NVP(fixture) );
  }

  std::cout << std::endl;


  return 0;
}
