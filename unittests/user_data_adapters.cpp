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
#include <cereal/archives/adapters.hpp>
#include <boost/test/unit_test.hpp>

struct SomeStruct {};

struct UserData
{
  UserData( SomeStruct * pp, SomeStruct & r ) :
    p(pp), ref(r) {}

  SomeStruct * p;
  std::reference_wrapper<SomeStruct> ref;
};

struct UserStruct
{
  UserStruct( std::int32_t i,
              SomeStruct * pointer,
              SomeStruct & reference ) :
    i32( i ),
    p( pointer ),
    ref( reference )
  { }

  std::int32_t i32;
  SomeStruct const * p;
  SomeStruct & ref;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( i32 );
  }

  template <class Archive>
  static void load_and_construct( Archive & ar, cereal::construct<UserStruct> & construct )
  {
    std::int32_t ii;
    ar( ii );
    auto & data = cereal::get_user_data<UserData>( ar );
    construct( ii, data.p, data.ref.get() );
    //construct( ii, ar.userdata.p, ar.userdata.ref.get() );
  }
};

template <class IArchive, class OArchive>
void test_user_data_adapters()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    SomeStruct ss;
    std::unique_ptr<UserStruct> o_ptr( new UserStruct( rng(), &ss, ss ) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_ptr);
    }

    decltype( o_ptr  ) i_ptr;

    std::istringstream is(os.str());
    {
      //UserData ud(&ss, ss);
      //cereal::UserDataAdapter<UserData, IArchive> iar(ud, is);
      IArchive iar(is);

      iar(i_ptr);
    }

    BOOST_CHECK_EQUAL( i_ptr->p == o_ptr->p, true );
    BOOST_CHECK_EQUAL( i_ptr->i32, o_ptr->i32 );
  }
}

BOOST_AUTO_TEST_CASE( binary_tuple )
{
  test_user_data_adapters<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_tuple )
{
  test_user_data_adapters<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_tuple )
{
  test_user_data_adapters<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_tuple )
{
  test_user_data_adapters<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

