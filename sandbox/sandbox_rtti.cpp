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

#include <type_traits>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sstream>
#include <fstream>
#include <iostream>

struct Base
{
  int y;
  virtual void foo() = 0;
  virtual ~Base() {}

  template<class Archive>
    void save(Archive & ar) const
    {
      std::cout << "Saving Base" << std::endl;
      ar( y );
    }

  template<class Archive>
    void load(Archive & ar)
    {
      std::cout << "Loading Base" << std::endl;
      ar( y );
    }
};

struct MyType : public Base
{
  virtual ~MyType() {}

  int x;

  void foo() {}

  template<class Archive>
    void save(Archive & ar) const
    {
      std::cout << "Saving MyType" << std::endl;
      ar( cereal::virtual_base_class<Base>( this ) );
    }

  template<class Archive>
    void load(Archive & ar)
    {
      std::cout << "Loading MyType" << std::endl;
      ar( cereal::base_class<Base>( this ) );
    }
};
CEREAL_REGISTER_TYPE(MyType)

struct YourType : public Base
{
  virtual ~YourType() {}

  YourType(int xx) : x(xx) {}
  YourType() : x(-1) {}
  int x;

  void foo() {}

  template<class Archive>
    void save(Archive & ar) const
    {
      std::cout << "Saving YourType" << std::endl;
      ar( x );
    }

  template<class Archive>
    void load(Archive & ar)
    {
      std::cout << "Loading YourType" << std::endl;
      ar( x );
    }
};

CEREAL_REGISTER_TYPE(YourType)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Base, YourType)

struct OurBase
{
  virtual void foo() {}

  template<class Archive>
    void serialize(Archive &)
    { }
};

struct OurType : public OurBase
{
  OurType() : OurBase(), x() {}
  OurType(int x_) : x(x_) {}
  virtual ~OurType() {}

  void foo() {}

  int x;

  template<class Archive>
    void serialize(Archive & ar)
    {
      ar( x );
    }
};

struct BaseVirtual
{
  int x;
  template <class Archive>
  void serialize( Archive & ar )
  { ar( x ); }
  virtual void foo() = 0;
};

struct DerivedVirtual : public virtual BaseVirtual
{
  virtual ~DerivedVirtual() {}

  int y;
  virtual void foo() {}

  template <class Archive>
  void save( Archive & ar ) const
  {
    ar( cereal::virtual_base_class<BaseVirtual>( this ) );
    ar( y );
  }

  template <class Archive>
  void load( Archive & ar )
  {
    ar( cereal::virtual_base_class<BaseVirtual>( this ) );
    ar( y );
  }
};

struct TestType
{
  int x;
  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x );
  }
};

namespace cereal
{
  template <class Archive> struct specialize<Archive, DerivedVirtual, cereal::specialization::member_load_save> {};
  template <class Archive> struct specialize<Archive, TestType, cereal::specialization::member_serialize> {};
}

struct AAA
{
  virtual void foo() = 0;
};

struct BBB : AAA
{
  virtual ~BBB() {}
  void foo() {}
  template <class Archive>
  void serialize( Archive & ) {}
};

CEREAL_REGISTER_TYPE(BBB)

template <class T> void nop(T&&) {}

int main()
{
  {
    std::ofstream ostream("rtti.txt");
    //cereal::BinaryOutputArchive oarchive(ostream);
    cereal::XMLOutputArchive oarchive(ostream);

    std::shared_ptr<Base> ptr1 = std::make_shared<MyType>();
    std::shared_ptr<Base> ptr2 = std::make_shared<YourType>(33);
    std::unique_ptr<Base> ptr3(new MyType());
    std::weak_ptr<Base>   ptr4 = ptr2;

    std::shared_ptr<OurType> ptr5 = std::make_shared<OurType>(99);

    oarchive(ptr1);
    oarchive(ptr2);
    oarchive(ptr3);
    oarchive(ptr4);
    oarchive(ptr5);

    //std::shared_ptr<AAA> a = std::make_shared<BBB>();
    //oarchive(a);
  }

  {
    std::ifstream istream("rtti.txt");
    //cereal::BinaryInputArchive iarchive(istream);
    cereal::XMLInputArchive iarchive(istream);

    std::shared_ptr<Base> ptr1;
    std::shared_ptr<Base> ptr2;
    std::unique_ptr<Base> ptr3;
    std::weak_ptr<Base>   ptr4;

    std::shared_ptr<OurType> ptr5;

    iarchive(ptr1);
    iarchive(ptr2);
    iarchive(ptr3);
    iarchive(ptr4);
    iarchive(ptr5);
  }
}
