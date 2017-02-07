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
    std::weak_ptr<PolyBase>   o_weak = o_shared;
    std::unique_ptr<PolyBase> o_unique( new PolyDerived( rngI(), rngF(), rngB(), rngD() ) );

    std::shared_ptr<PolyBaseA> o_sharedA = std::make_shared<PolyDerivedD>( random_basic_string<char>(gen),
                                                                           rngD(), rngI(), rngL() );

    std::weak_ptr<PolyBaseA>   o_weakA = o_sharedA;
    std::unique_ptr<PolyBaseA> o_uniqueA( new PolyDerivedD( random_basic_string<char>(gen),
                                                            rngD(), rngI(), rngL() ) );

    auto pda = std::make_shared<PolyDerivedLA>( rngI() );
    pda->vec.emplace_back( std::make_shared<PolyDerivedLA>( rngI() ) );
    std::shared_ptr<PolyLA>   o_sharedLA = pda;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_shared, o_weak, o_unique );
      oar( o_sharedLA );

      oar( o_sharedA, o_weakA, o_uniqueA );
    }

    decltype(o_shared) i_shared;
    decltype(o_weak) i_weak;
    decltype(o_unique) i_unique;

    decltype(o_sharedLA) i_sharedLA;

    decltype(o_sharedA) i_sharedA;
    decltype(o_weakA) i_weakA;
    decltype(o_uniqueA) i_uniqueA;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_shared, i_weak, i_unique );
      iar( i_sharedLA );
      iar( i_sharedA, i_weakA, i_uniqueA );
    }

    auto i_locked = i_weak.lock();
    auto o_locked = o_weak.lock();

    auto i_sharedLA2 = i_sharedLA->shared_from_this();

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

    CHECK_EQ(*dynamic_cast<PolyDerivedLA*>(i_sharedLA.get()), *dynamic_cast<PolyDerivedLA*>(o_sharedLA.get()));
    CHECK_EQ(*dynamic_cast<PolyDerivedLA*>(i_sharedLA2.get()), *dynamic_cast<PolyDerivedLA*>(o_sharedLA.get()));

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

#endif // CEREAL_TEST_POLYMORPHIC_H_
