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
#include <cereal/archives/json.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include <cereal/external/rapidjson/filestream.h>

#include <cxxabi.h>
#include <sstream>
#include <fstream>
#include <cassert>
#include <complex>
#include <iostream>
#include <iomanip>

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
    void change()
    {
      a = 4;
      b = 4;
      c = 4;
      d = 4;
      s = "4";
    }
};

struct Fixture
{
  SubFixture f1, f2, f3;
  int array[4] = {1, 2, 3, 4};

  template<class Archive>
  void save(Archive & ar) const
  {
    ar( f1,
        CEREAL_NVP(f2),
        f3 );
    ar.saveBinaryValue( array, sizeof(int)*4, "cool array man" );
  }

  template<class Archive>
  void load(Archive & ar)
  {
    ar( f1,
        CEREAL_NVP(f2),
        f3 );
    ar.loadBinaryValue( array, sizeof(int)*4 );
  }

    void change()
    {
      f1.change();
      f2.change();
      f3.change();
    }
};

struct AAA
{
  int one = 1, two = 2;

  std::vector<std::vector<int>> three = {{1,2,3}, {4,5,6}, {}};

  template<class Archive>
    void serialize(Archive & ar)
    {
      ar( CEREAL_NVP(one), CEREAL_NVP(two) );
      //ar( CEREAL_NVP(three) );
    }
};

class Stuff
{
  public:
    Stuff() = default;

    void fillData()
    {
      data = { {"imaginary", {{0, -1.0f},
                              {0, -2.9932f},
                              {0, -3.5f}}},
               {"real", {{1.0f, 0},
                         {2.2f, 0},
                         {3.3f, 0}}} };
    }

  private:
    std::map<std::string, std::vector<std::complex<float>>> data;

    friend class cereal::access;

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( CEREAL_NVP(data) );
    }
};

// ######################################################################
int main()
{
  std::cout << std::boolalpha << std::endl;

  {
    std::ofstream os("file.json");
    cereal::JSONOutputArchive oar( os, 5 );

    //auto f = std::make_shared<Fixture>();
    //auto f2 = f;
    //oar( f );
    //oar( f2 );
    Stuff s; s.fillData();
    oar( cereal::make_nvp("best data ever", s) );
  }

  {
    std::ifstream is("file.json");
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    std::cout << "---------------------" << std::endl << str << std::endl << "---------------------" << std::endl;
  }

  {
  cereal::JSONOutputArchive archive( std::cout );
  bool arr[] = {true, false};
  std::vector<int> vec = {1, 2, 3, 4, 5};
  archive( CEREAL_NVP(vec),
           arr );
  }


  //{
  //  std::ifstream is("file.json");
  //  cereal::JSONInputArchive iar( is );

  //  std::shared_ptr<Fixture> f, f2;
  //  iar( f, f2 );
  //  assert( f->array[0] == 1 );
  //  assert( f->array[1] == 2 );
  //  assert( f->array[2] == 3 );
  //  assert( f->array[3] == 4 );
  //}

  return 0;
}
