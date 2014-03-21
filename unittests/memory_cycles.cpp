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
#include "common.hpp"
#include <boost/test/unit_test.hpp>

struct MemoryCycle
{
  MemoryCycle() = default;

  MemoryCycle( int v ) :
    value( v )
  { }

  int value;
  std::weak_ptr<MemoryCycle> ptr;

  bool operator==( MemoryCycle const & other ) const
  {
    return value == other.value && ptr.lock() == other.ptr.lock();
  }

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( value, ptr );
  }
};

std::ostream& operator<<(std::ostream& os, MemoryCycle const & s)
{
  os << "[value: " << s.value << " ptr: " << s.ptr.lock() << "]";
  return os;
}

class MemoryCycleLoadAndConstruct
{
  public:
    MemoryCycleLoadAndConstruct( int v ) :
      value( v )
    { }

    MemoryCycleLoadAndConstruct( int v,
        std::weak_ptr<MemoryCycleLoadAndConstruct> p ) :
      value( v ),
      ptr( p )
    { }

    bool operator==( MemoryCycleLoadAndConstruct const & other ) const
    {
      return value == other.value && ptr.lock() == other.ptr.lock();
    }

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( value, ptr );
    }

    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<MemoryCycleLoadAndConstruct> & construct )
    {
      int value;
      std::weak_ptr<MemoryCycleLoadAndConstruct> ptr;

      ar( value, ptr );
      construct( value, ptr );
    }

    int value;
    std::weak_ptr<MemoryCycleLoadAndConstruct> ptr;
};

std::ostream& operator<<(std::ostream& os, MemoryCycleLoadAndConstruct const & s)
{
  os << "[value: " << s.value << " ptr: " << s.ptr.lock() << "]";
  return os;
}

template <class IArchive, class OArchive>
void test_memory_cycles()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    auto o_ptr1 = std::make_shared<MemoryCycle>( random_value<int>(gen) );
    o_ptr1->ptr = o_ptr1;
    auto o_ptr2 = std::make_shared<MemoryCycleLoadAndConstruct>( random_value<int>(gen) );
    o_ptr2->ptr = o_ptr2;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_ptr1 );
      oar( o_ptr2 );
    }

    decltype(o_ptr1) i_ptr1;
    decltype(o_ptr2) i_ptr2;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_ptr1 );
      iar( i_ptr2 );
    }

    BOOST_CHECK_EQUAL( o_ptr1->value, i_ptr1->value );
    BOOST_CHECK_EQUAL( i_ptr1.get(), i_ptr1->ptr.lock().get() );
    BOOST_CHECK_EQUAL( o_ptr2->value, i_ptr2->value );
    BOOST_CHECK_EQUAL( i_ptr2.get(), i_ptr2->ptr.lock().get() );
  }
}

BOOST_AUTO_TEST_CASE( binary_memory_cycles )
{
  test_memory_cycles<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_memory_cycles )
{
  test_memory_cycles<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_memory_cycles )
{
  test_memory_cycles<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_memory_cycles )
{
  test_memory_cycles<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

