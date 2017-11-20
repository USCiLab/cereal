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
#ifndef CEREAL_TEST_POLYMORPHIC_H_
#define CEREAL_TEST_POLYMORPHIC_H_
#include "common.hpp"

#if CEREAL_THREAD_SAFE
#include <future>
#endif

struct PolyBaseA
{
  virtual void foo() = 0;
  virtual ~PolyBaseA() {}
};

struct PolyBaseAA : PolyBaseA
{
  PolyBaseAA() {}
  PolyBaseAA( long ww ) : w(ww) {}
  virtual ~PolyBaseAA() {}
  long w;

  void foo() {}

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( w );
  }

  bool operator==( PolyBaseAA const & other ) const
  {
    return w == other.w;
  }
};

CEREAL_REGISTER_POLYMORPHIC_RELATION(PolyBaseA, PolyBaseAA)

struct PolyBaseB : virtual PolyBaseAA
{
  PolyBaseB() {}
  PolyBaseB( int xx, long ww ) : PolyBaseAA(ww), x(xx) {}
  virtual ~PolyBaseB() {}
  int x;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( cereal::virtual_base_class<PolyBaseAA>( this ) );
    ar( x );
  }

  bool operator==( PolyBaseB const & other ) const
  {
    return PolyBaseAA::operator==( other ) &&
           x == other.x;
  }
};

struct PolyBaseC : virtual PolyBaseAA
{
  PolyBaseC() {}
  PolyBaseC( double yy, long ww ) : PolyBaseAA(ww), y(yy) {}
  virtual ~PolyBaseC() {}
  double y;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( cereal::virtual_base_class<PolyBaseAA>( this ) );
    ar( y );
  }

  bool operator==( PolyBaseC const & other ) const
  {
    return PolyBaseAA::operator==( other ) &&
           std::abs(y - other.y) < 1e-5;
  }
};

struct PolyDerivedD : PolyBaseB, PolyBaseC
{
  PolyDerivedD() {}
  PolyDerivedD( std::string const & zz, double yy, int xx, long ww ) :
    PolyBaseAA( ww ), PolyBaseB( xx, ww ), PolyBaseC( yy, ww ), z(zz) {}
  virtual ~PolyDerivedD() {}
  std::string z;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( cereal::base_class<PolyBaseB>( this ) );
    ar( cereal::base_class<PolyBaseC>( this ) );
    ar( z );
  }

  bool operator==( PolyDerivedD const & other ) const
  {
    return PolyBaseB::operator==( other ) &&
           PolyBaseC::operator==( other ) &&
           z == other.z;
  }
};

CEREAL_REGISTER_TYPE(PolyDerivedD)

struct PolyBase
{
  PolyBase() {}
  PolyBase( int xx, float yy ) : x(xx), y(yy) {}
  virtual ~PolyBase() {}
  int x;
  float y;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x, y );
  }

  virtual void foo() = 0;

  bool operator==( PolyBase const & other ) const
  {
    return x == other.x && std::abs(y - other.y) < 1e-5;
  }
};

struct PolyDerived : PolyBase
{
  PolyDerived() {}
  PolyDerived( int xx, float yy, bool aa, double bb ) :
    PolyBase( xx, yy ), a(aa), b(bb) {}
  virtual ~PolyDerived() {}

  bool a;
  double b;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( cereal::base_class<PolyBase>( this ),
        a, b );
  }

  bool operator==( PolyDerived const & other ) const
  {
    return PolyBase::operator==( other ) && a == other.a && std::abs(b - other.b) < 1e-5;
  }

  void foo() {}
};

CEREAL_REGISTER_TYPE(PolyDerived)

struct PolyLA : std::enable_shared_from_this<PolyLA>
{
  PolyLA() {}
  virtual ~PolyLA() {}
  virtual void foo() = 0;
};

struct PolyDerivedLA : public PolyLA
{
  PolyDerivedLA( int xx ) : x( xx ) { }
  virtual ~PolyDerivedLA() {}

  int x;
  std::vector<std::shared_ptr<PolyLA>> vec;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x );
    ar( vec );
  }

  template <class Archive>
  static void load_and_construct( Archive & ar, cereal::construct<PolyDerivedLA> & construct )
  {
    int xx;
    ar( xx );
    construct( xx );
    ar( construct->vec );
  }

  void foo() {}

  bool operator==( PolyDerivedLA const & other ) const
  {
    if( x != other.x )
      return false;
    if( vec.size() != other.vec.size() )
      return false;
    for( size_t i = 0; i < vec.size(); ++i )
      if( !(*std::dynamic_pointer_cast<PolyDerivedLA>(vec[i]) == *std::dynamic_pointer_cast<PolyDerivedLA>(other.vec[i])) )
        return false;

    return true;
  }
};

CEREAL_REGISTER_TYPE(PolyDerivedLA)
CEREAL_REGISTER_POLYMORPHIC_RELATION(PolyLA, PolyDerivedLA)

std::ostream& operator<<(std::ostream& os, PolyDerivedLA const & s)
{
    os << "[x: " << s.x << "] ";
    for( auto const & v : s.vec )
      os << " child: " << (*std::dynamic_pointer_cast<PolyDerivedLA>(v));
    return os;
}

std::ostream& operator<<(std::ostream& os, PolyDerived const & s)
{
    os << "[x: " << s.x << " y: " << s.y << " a: " << s.a << " b: " << s.b << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, PolyDerivedD const & s)
{
    os << "[w: " << s.w << " x: " << s.x << " y: " << s.y << " z: " << s.z << "]";
    return os;
}

template <class IArchive, class OArchive> inline
void test_polymorphic()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  #if CEREAL_THREAD_SAFE
  static std::mutex testMutex;
  #endif

  auto rngB = [&](){ return random_value<int>( gen ) % 2 == 0; };
  auto rngI = [&](){ return random_value<int>( gen ); };
  auto rngL = [&](){ return random_value<long>( gen ); };
  auto rngF = [&](){ return random_value<float>( gen ); };
  auto rngD = [&](){ return random_value<double>( gen ); };

  for(int ii=0; ii<100; ++ii)
  {
    std::shared_ptr<PolyBase> o_shared = std::make_shared<PolyDerived>( rngI(), rngF(), rngB(), rngD() );
    std::shared_ptr<const PolyBase> o_sharedC = std::make_shared<const PolyDerived>( rngI(), rngF(), rngB(), rngD() );

    std::weak_ptr<PolyBase>   o_weak = o_shared;
    std::weak_ptr<const PolyBase>   o_weakC = o_sharedC;

    std::unique_ptr<PolyBase> o_unique( new PolyDerived( rngI(), rngF(), rngB(), rngD() ) );
    std::unique_ptr<const PolyBase> o_uniqueC( new PolyDerived( rngI(), rngF(), rngB(), rngD() ) );

    std::shared_ptr<PolyBaseA> o_sharedA = std::make_shared<PolyDerivedD>( random_basic_string<char>(gen),
                                                                           rngD(), rngI(), rngL() );

    std::weak_ptr<PolyBaseA>   o_weakA = o_sharedA;
    std::unique_ptr<PolyBaseA> o_uniqueA( new PolyDerivedD( random_basic_string<char>(gen),
                                                            rngD(), rngI(), rngL() ) );

    auto pda = std::make_shared<PolyDerivedLA>( rngI() );
    pda->vec.emplace_back( std::make_shared<PolyDerivedLA>( rngI() ) );
    std::shared_ptr<PolyLA>   o_sharedLA = pda;

    auto pdaC = std::make_shared<const PolyDerivedLA>( rngI() );
    pda->vec.emplace_back( std::make_shared<PolyDerivedLA>( rngI() ) );
    std::shared_ptr<const PolyLA>   o_sharedLAC = pdaC;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_shared, o_sharedC );
      oar( o_weak, o_weakC );
      oar( o_unique, o_uniqueC );

      oar( o_sharedLA );
      oar( o_sharedLAC );

      oar( o_sharedA, o_weakA, o_uniqueA );
    }

    decltype(o_shared) i_shared;
    decltype(o_sharedC) i_sharedC;

    decltype(o_weak) i_weak;
    decltype(o_weakC) i_weakC;

    decltype(o_unique) i_unique;
    decltype(o_uniqueC) i_uniqueC;

    decltype(o_sharedLA) i_sharedLA;
    decltype(o_sharedLAC) i_sharedLAC;

    decltype(o_sharedA) i_sharedA;
    decltype(o_weakA) i_weakA;
    decltype(o_uniqueA) i_uniqueA;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_shared, i_sharedC );
      iar( i_weak, i_weakC );
      iar( i_unique, i_uniqueC );

      iar( i_sharedLA );
      iar( i_sharedLAC );

      iar( i_sharedA, i_weakA, i_uniqueA );
    }

    auto i_locked = i_weak.lock();
    auto o_locked = o_weak.lock();

    auto i_lockedC = i_weakC.lock();
    auto o_lockedC = o_weakC.lock();

    auto i_sharedLA2 = i_sharedLA->shared_from_this();
    auto i_sharedLA2C = i_sharedLAC->shared_from_this();

    auto i_lockedA = i_weakA.lock();
    auto o_lockedA = o_weakA.lock();

    #if CEREAL_THREAD_SAFE
    std::lock_guard<std::mutex> lock( testMutex );
    #endif

    CHECK_EQ(i_shared.get(), i_locked.get());
    CHECK_EQ(*dynamic_cast<PolyDerived*>(i_shared.get()), *dynamic_cast<PolyDerived*>(o_shared.get()));
    CHECK_EQ(*dynamic_cast<PolyDerived*>(i_shared.get()), *dynamic_cast<PolyDerived*>(i_locked.get()));
    CHECK_EQ(*dynamic_cast<PolyDerived*>(i_locked.get()), *dynamic_cast<PolyDerived*>(o_locked.get()));
    CHECK_EQ(*dynamic_cast<PolyDerived*>(i_unique.get()), *dynamic_cast<PolyDerived*>(o_unique.get()));

    CHECK_EQ(i_sharedC.get(), i_lockedC.get());
    CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_sharedC.get()), *dynamic_cast<const PolyDerived*>(o_sharedC.get()));
    CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_sharedC.get()), *dynamic_cast<const PolyDerived*>(i_lockedC.get()));
    CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_lockedC.get()), *dynamic_cast<const PolyDerived*>(o_lockedC.get()));
    CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_uniqueC.get()), *dynamic_cast<const PolyDerived*>(o_uniqueC.get()));

    CHECK_EQ(*dynamic_cast<PolyDerivedLA*>(i_sharedLA.get()), *dynamic_cast<PolyDerivedLA*>(o_sharedLA.get()));
    CHECK_EQ(*dynamic_cast<PolyDerivedLA*>(i_sharedLA2.get()), *dynamic_cast<PolyDerivedLA*>(o_sharedLA.get()));

    CHECK_EQ(*dynamic_cast<const PolyDerivedLA*>(i_sharedLAC.get()), *dynamic_cast<const PolyDerivedLA*>(o_sharedLAC.get()));
    CHECK_EQ(*dynamic_cast<const PolyDerivedLA*>(i_sharedLA2C.get()), *dynamic_cast<const PolyDerivedLA*>(o_sharedLAC.get()));

    CHECK_EQ(i_sharedA.get(), i_lockedA.get());
    CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_sharedA.get()), *dynamic_cast<PolyDerivedD*>(o_sharedA.get()));
    CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_sharedA.get()), *dynamic_cast<PolyDerivedD*>(i_lockedA.get()));
    CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_lockedA.get()), *dynamic_cast<PolyDerivedD*>(o_lockedA.get()));
    CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_uniqueA.get()), *dynamic_cast<PolyDerivedD*>(o_uniqueA.get()));
  }
}

#if CEREAL_THREAD_SAFE
template <class IArchive, class OArchive> inline
void test_polymorphic_threading()
{
  std::vector<std::future<bool>> pool;
  for( size_t i = 0; i < 100; ++i )
    pool.emplace_back( std::async( std::launch::async,
                                   [](){ test_polymorphic<IArchive, OArchive>(); return true; } ) );

  for( auto & future : pool )
    future.wait();

  for( auto & future : pool )
    CHECK_UNARY( future.get() );
}
#endif // CEREAL_THREAD_SAFE

struct Object
{
  Object() = default;
  Object( int xx ) : x(xx) {}

  virtual ~Object() {}
  virtual void func() {}

  int x;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x );
  }
};

#define CEREAL_TEST_CREATE_DERIVED_CLASS(Base, Derived) \
struct Derived : public Base                            \
{                                                       \
  Derived() = default;                                  \
  Derived( int yy ) : Base( yy ), Derived##y( yy ) {}   \
  virtual ~Derived() {}                                 \
                                                        \
  virtual void func() override {}                       \
                                                        \
  int Derived##y;                                       \
  template <class Archive>                              \
  void serialize( Archive & ar )                        \
  {                                                     \
    ar( cereal::base_class<Base>( this ), Derived##y ); \
  }                                                     \
};                                                      \
CEREAL_REGISTER_TYPE(Derived)

//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived0)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived0,Derived1)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived1,Derived2)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived2,Derived3)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived3,Derived4)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived5)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived5,Derived6)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived6,Derived7)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived7,Derived8)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived8,Derived9)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived10)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived10,Derived11)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived11,Derived12)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived12,Derived13)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived13,Derived14)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived15)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived15,Derived16)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived16,Derived17)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived17,Derived18)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived18,Derived19)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived20)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived20,Derived21)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived21,Derived22)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived22,Derived23)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived23,Derived24)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived25)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived25,Derived26)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived26,Derived27)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived27,Derived28)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived28,Derived29)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived30)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived30,Derived31)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived31,Derived32)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived32,Derived33)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived33,Derived34)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived35)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived35,Derived36)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived36,Derived37)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived37,Derived38)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived38,Derived39)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived40)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived40,Derived41)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived41,Derived42)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived42,Derived43)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived43,Derived44)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived45)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived45,Derived46)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived46,Derived47)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived47,Derived48)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived48,Derived49)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived50)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived50,Derived51)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived51,Derived52)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived52,Derived53)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived53,Derived54)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived55)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived55,Derived56)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived56,Derived57)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived57,Derived58)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived58,Derived59)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived60)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived60,Derived61)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived61,Derived62)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived62,Derived63)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived63,Derived64)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived65)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived65,Derived66)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived66,Derived67)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived67,Derived68)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived68,Derived69)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived70)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived70,Derived71)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived71,Derived72)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived72,Derived73)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived73,Derived74)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived75)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived75,Derived76)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived76,Derived77)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived77,Derived78)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived78,Derived79)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived80)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived80,Derived81)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived81,Derived82)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived82,Derived83)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived83,Derived84)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived85)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived85,Derived86)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived86,Derived87)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived87,Derived88)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived88,Derived89)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived90)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived90,Derived91)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived91,Derived92)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived92,Derived93)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived93,Derived94)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived95)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived95,Derived96)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived96,Derived97)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived97,Derived98)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived98,Derived99)

//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived0)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived0,Derived1)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived1,Derived2)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived2,Derived3)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived0,Derived4)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived4,Derived5)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived5,Derived6)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived0,Derived7)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived7,Derived8)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived8,Derived9)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived10)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived10,Derived11)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived11,Derived12)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived12,Derived13)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived10,Derived14)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived14,Derived15)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived15,Derived16)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived10,Derived17)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived17,Derived18)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived18,Derived19)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived20)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived20,Derived21)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived21,Derived22)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived22,Derived23)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived20,Derived24)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived24,Derived25)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived25,Derived26)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived20,Derived27)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived27,Derived28)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived28,Derived29)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived30)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived30,Derived31)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived31,Derived32)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived32,Derived33)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived30,Derived34)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived34,Derived35)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived35,Derived36)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived30,Derived37)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived37,Derived38)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived38,Derived39)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived40)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived40,Derived41)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived41,Derived42)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived42,Derived43)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived40,Derived44)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived44,Derived45)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived45,Derived46)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived40,Derived47)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived47,Derived48)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived48,Derived49)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived50)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived50,Derived51)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived51,Derived52)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived52,Derived53)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived50,Derived54)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived54,Derived55)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived55,Derived56)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived50,Derived57)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived57,Derived58)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived58,Derived59)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived60)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived60,Derived61)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived61,Derived62)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived62,Derived63)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived60,Derived64)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived64,Derived65)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived65,Derived66)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived60,Derived67)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived67,Derived68)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived68,Derived69)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived70)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived70,Derived71)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived71,Derived72)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived72,Derived73)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived70,Derived74)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived74,Derived75)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived75,Derived76)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived70,Derived77)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived77,Derived78)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived78,Derived79)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived80)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived80,Derived81)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived81,Derived82)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived82,Derived83)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived80,Derived84)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived84,Derived85)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived85,Derived86)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived80,Derived87)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived87,Derived88)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived88,Derived89)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Object,Derived90)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived90,Derived91)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived91,Derived92)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived92,Derived93)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived90,Derived94)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived94,Derived95)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived95,Derived96)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived90,Derived97)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived97,Derived98)
//CEREAL_TEST_CREATE_DERIVED_CLASS(Derived98,Derived99)


#endif // CEREAL_TEST_POLYMORPHIC_H_
