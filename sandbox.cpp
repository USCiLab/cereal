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
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/boost_variant.hpp>

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
    ar(CEREAL_NVP(a));
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

  int x[5];

  {
    std::ostringstream os;
    cereal::BinaryOutputArchive archive(os);
    archive( x );
  }


  //Everything e_out;
  //e_out.x = 99;
  //e_out.y = 100;
  //e_out.t1 = {1};
  //e_out.t2 = {2};
  //e_out.t3 = {3};
  //e_out.t4 = {4};
  //e_out.s = "Hello, World!";

  //Test2 t2 = {22};

  //{
  //  std::ofstream os("out.txt");
  //  cereal::BinaryOutputArchive archive(os);
  //  archive(CEREAL_NVP(e_out));
  //  archive(t2);
  //}

  //Everything e_in;

  //{
  //  std::ifstream is("out.txt");
  //  cereal::BinaryInputArchive archive(is);
  //  archive(CEREAL_NVP(e_in));
  //  archive(t2);
  //}

  //assert(e_in == e_out);//
  //
  //{                     //
  //  std::ofstream os("pt//r.txt");
  //  cereal::BinaryOutput//Archive archive(os);
  //  std::shared_ptr<std://:shared_ptr<int>> xptr1 = std::make_shared<std::shared_ptr<int>>(std::make_shared<int>(5));
  //  std::shared_ptr<int>// xptr2 = *xptr1;
  //  std::weak_ptr<int> w//ptr2 = xptr2;
  //  std::unique_ptr<Test//1> uptr(new Test1);
  //  uptr->a = 99;       //
  //  archive & xptr1;    //
  //  archive & xptr2;    //
  //  archive & wptr2;    //
  //  archive & uptr;     //
  //}

  //{
  //  std::ifstream is("ptr.txt");
  //  cereal::BinaryInputArchive archive(is);
  //  std::shared_ptr<std::shared_ptr<int>> xptr1;
  //  std::shared_ptr<int> xptr2;
  //  std::weak_ptr<int> wptr2;
  //  std::unique_ptr<Test1> uptr;
  //  archive & xptr1;
  //  archive & xptr2;
  //  archive & wptr2;
  //  archive & uptr;

  //  std::cout << **xptr1 << std::endl;
  //  std::cout << *xptr2 << std::endl;
  //  std::cout << (*xptr1).get() << " == " << xptr2.get() << " ? " << ((*xptr1).get() == xptr2.get()) << std::endl;
  //  std::cout << *(wptr2.lock()) << std::endl;
  //  std::cout << (wptr2.lock().get() == xptr2.get()) << std::endl;
  //  std::cout << uptr->a << std::endl;
  //}

  //{
  //  std::ofstream os("arr.txt");
  //  cereal::BinaryOutputArchive archive(os);
  //  int a1[] = {1, 2, 3};
  //  int a2[][2] = {{4, 5}, {6, 7}};
  //  archive & a1;
  //  archive & a2;
  //  EmptyStruct empty;
  //  archive & empty;
  //  archive & std::complex<float>();
  //}

  //{
  //  std::ifstream is("arr.txt");
  //  cereal::BinaryInputArchive archive(is);
  //  int a1[3];
  //  int a2[2][2];
  //  archive & a1;
  //  archive & a2;

  //  for(auto i : a1)
  //    std::cout << i << " ";
  //  std::cout << std::endl;
  //  for( auto const & i : a2 )
  //  {
  //    for( auto j : i )
  //      std::cout << j << " ";
  //    std::cout << std::endl;
  //  }
  //  std::cout << std::endl;
  //}


  //std::ostringstream os;
  //cereal::BinaryOutputArchive out_archive(os);

  //in_archive & nd;

  //std::cout << nd->y << std::endl;

  //auto zxx = cereal::access::load_and_allocate<NonEmptyStruct>( out_archive );
  //auto xxx = cereal::access::load_and_allocate<NoDefaultCtor>( out_archive );

  //std::cout << cereal::traits::has_member_load_and_allocate<NoDefaultCtor, cereal::BinaryOutputArchive>() << std::endl;
  //std::cout << cereal::traits::has_member_load_and_allocate<NonEmptyStruct, cereal::BinaryOutputArchive>() << std::endl;
  //std::cout << cereal::traits::has_member_load_and_allocate<int, cereal::BinaryOutputArchive>() << std::endl;

  //cereal::Construct<int>::Create( out_archive );
  //cereal::Construct<NoDefaultCtor>::Create( out_archive );

  //std::cout << cereal::traits::has_non_member_load_and_allocate<NoDefaultCtor, cereal::BinaryOutputArchive>() << std::endl;
  //std::cout << cereal::traits::has_non_member_load_and_allocate<NonEmptyStruct, cereal::BinaryOutputArchive>() << std::endl;
  //std::cout << cereal::traits::has_non_member_load_and_allocate<int, cereal::BinaryOutputArchive>() << std::endl;

  //auto p = std::make_shared<NoDefaultCtor>( 5 );
  //out_archive & p;

  //std::istringstream is(os.str());
  //cereal::BinaryInputArchive in_archive(is);

  //p->y = 3;

  //in_archive & p;
  //std::cout << p->y << std::endl;

  return 0;
}
