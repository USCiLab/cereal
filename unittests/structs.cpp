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
void test_structs()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    StructInternalSerialize o_iser = { random_value<int>(gen), random_value<int>(gen) };
    StructInternalSplit     o_ispl = { random_value<int>(gen), random_value<int>(gen) };
    StructExternalSerialize o_eser = { random_value<int>(gen), random_value<int>(gen) };
    StructExternalSplit     o_espl = { random_value<int>(gen), random_value<int>(gen) };

    std::ostringstream os;
    {
      OArchive oar(os);
      oar( o_iser, o_ispl, o_eser, o_espl);
    }

    StructInternalSerialize i_iser;
    StructInternalSplit     i_ispl;
    StructExternalSerialize i_eser;
    StructExternalSplit     i_espl;

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      iar( i_iser, i_ispl, i_eser, i_espl);
    }

    BOOST_CHECK(i_iser == o_iser);
    BOOST_CHECK(i_ispl == o_ispl);
    BOOST_CHECK(i_eser == o_eser);
    BOOST_CHECK(i_espl == o_espl);
  }
}

BOOST_AUTO_TEST_CASE( binary_structs )
{
  test_structs<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_structs )
{
  test_structs<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_structs )
{
  test_structs<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_structs )
{
  test_structs<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

BOOST_AUTO_TEST_CASE( message_pack_structs )
{
  test_structs<cereal::MessagePackInputArchive, cereal::MessagePackOutputArchive>();
}
