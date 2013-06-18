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
#include <cereal/binary_archive/binary_archive.hpp>
#include <cereal/binary_archive/string.hpp>
#include <cereal/binary_archive/utility.hpp>
#include <cereal/binary_archive/memory.hpp>
#include <cereal/binary_archive/complex.hpp>

#include <cxxabi.h>
#include <sstream>
#include <fstream>
#include <cassert>
#include <complex>
#include <iostream>

// ###################################
struct Test1
{
  int a;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar & CEREAL_NVP(a);
  }
};

// ###################################
class Test2
{
  public:
    int a;

  private:
    friend class cereal::access;

    template<class Archive>
      void save(Archive & ar) const
      {
        ar & CEREAL_NVP(a);
      }

    template<class Archive>
      void load(Archive & ar)
      {
        ar & CEREAL_NVP(a);
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
  ar & CEREAL_NVP(t.a);
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
    ar & CEREAL_NVP(t.a);
  }

  template<class Archive>
  void load(Archive & ar, Test4 & t)
  {
    ar & CEREAL_NVP(t.a);
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
        ar & a;
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
    ar & CEREAL_NVP(x);
    ar & CEREAL_NVP(y);
    ar & CEREAL_NVP(t1);
    ar & CEREAL_NVP(t2);
    ar & CEREAL_NVP(t3);
    ar & CEREAL_NVP(t4);
    ar & CEREAL_NVP(s);
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

struct Base
{
  int b;

  Base() {};
  Base(int x) : b(x) {}

  template<class Archive>
  void serialize(Archive & ar)
  {
    std::cout << "Begin: serializing Base" << std::endl;

    ar & b;

    std::cout << "End  : serializing Base" << std::endl;
  }
};

struct MiddleA : public virtual Base
{
  int ma;

  MiddleA() {};
  MiddleA(int x) : /*Base(x+1),*/ ma(x) {}

  template<class Archive>
  void serialize(Archive & ar)
  {
    std::cout << "Begin: serializing MiddleA" << std::endl;

    ar & cereal::base_class<Base>(this);
    ar & ma;

    std::cout << "End  : serializing MiddleA" << std::endl;
  }
};

struct MiddleB : public virtual Base
{
  int mb;

  MiddleB() {};
  MiddleB(int x) : Base(x+1), mb(x) {}

  template<class Archive>
  void serialize(Archive & ar)
  {
    std::cout << "Begin: serializing MiddleB" << std::endl;

    ar & cereal::base_class<Base>(this);
    ar & mb;

    std::cout << "End  : serializing MiddleB" << std::endl;
  }
};

struct Derived : public MiddleA, public MiddleB
{
  int d;

  Derived() {};
  Derived(int x) : MiddleA(x+1), MiddleB(x+2), Base(x+3), d(x) {};

  template<class Archive>
  void serialize(Archive & ar)
  {
    std::cout << "Begin: serializing Derived" << std::endl;

    ar & cereal::base_class<MiddleA>(this);
    ar & cereal::base_class<MiddleB>(this);
    ar & d;

    std::cout << "End  : serializing Derived" << std::endl;
  }

  void print()
  {
    std::cout << d << " " << ma << " " << mb << " " << b << std::endl;
  }
};

struct MyHash 
{
  size_t operator()(std::pair<std::type_index, void const *> const & p) const
  {
    return std::hash<std::type_index>()(p.first) ^ (std::hash<void const *>()(p.second) << 1);
  }
};

// ######################################################################
int main()
{

  {
    Derived d1(0);
    Derived d2(100);
    d1.print();
    d2.print();
    std::ofstream os("out.txt");
    cereal::BinaryOutputArchive archive(os);
    archive & d1;
    archive & d2;
  }

  {
    Derived d1;
    Derived d2;
    std::ifstream is("out.txt");
    cereal::BinaryInputArchive archive(is);
    archive & d1;
    archive & d2;

    d1.print();
    d2.print();
  }

  return 0;
}
