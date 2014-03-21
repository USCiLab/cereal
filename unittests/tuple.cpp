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

template <class IArchive, class OArchive>
void test_tuple()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    auto o_podtuple = std::make_tuple( rng(), rng(), rng(), rng() );
    auto o_isertuple = std::make_tuple( StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ),
        StructInternalSerialize( rng(), rng() ) );
    auto o_ispltuple = std::make_tuple( StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ),
        StructInternalSplit( rng(), rng() ) );
    auto o_esertuple = std::make_tuple( StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ) );
    auto o_espltuple = std::make_tuple( StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ),
        StructExternalSerialize( rng(), rng() ) );

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podtuple);
      oar(o_isertuple);
      oar(o_ispltuple);
      oar(o_esertuple);
      oar(o_espltuple);
    }

    decltype( o_podtuple  ) i_podtuple;
    decltype( o_isertuple ) i_isertuple;
    decltype( o_ispltuple ) i_ispltuple;
    decltype( o_esertuple ) i_esertuple;
    decltype( o_espltuple ) i_espltuple;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podtuple);
      iar(i_isertuple);
      iar(i_ispltuple);
      iar(i_esertuple);
      iar(i_espltuple);
    }

    BOOST_CHECK_EQUAL( i_podtuple == o_podtuple, true );
    BOOST_CHECK_EQUAL( i_isertuple == o_isertuple, true );
    BOOST_CHECK_EQUAL( i_ispltuple == o_ispltuple, true );
    BOOST_CHECK_EQUAL( i_esertuple == o_esertuple, true );
    BOOST_CHECK_EQUAL( i_espltuple == o_espltuple, true );
  }
}

BOOST_AUTO_TEST_CASE( binary_tuple )
{
  test_tuple<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_tuple )
{
  test_tuple<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_tuple )
{
  test_tuple<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_tuple )
{
  test_tuple<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

