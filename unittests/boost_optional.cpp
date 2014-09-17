/*
  Copyright (c) 2014, Steve Hickman
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
  DISCLAIMED. IN NO EVENT SHALL STEVE HICKMAN BE LIABLE FOR ANY
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
void test_opt()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto rng = [&](){ return random_value<int>(gen); };

  for(int ii=0; ii<100; ++ii)
  {
    boost::optional<int> o_podOptEmpty;
    boost::optional<int> o_podopt = {rng()};
    boost::optional<StructInternalSerialize> o_iseropt = { { rng(), rng() } };
    boost::optional<StructInternalSplit>     o_isplopt = { { rng(), rng() }};
    boost::optional<StructExternalSerialize> o_eseropt = { { rng(), rng() } };
    boost::optional<StructExternalSplit>     o_esplopt = { { rng(), rng() } };

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podOptEmpty);
      oar(o_podopt);
      oar(o_iseropt);
      oar(o_isplopt);
      oar(o_eseropt);
      oar(o_esplopt);
    }

    boost::optional<int> i_podOptEmpty;
    boost::optional<int> i_podopt;
    boost::optional<StructInternalSerialize>  i_iseropt;
    boost::optional<StructInternalSplit>      i_isplopt;
    boost::optional<StructExternalSerialize>  i_eseropt;
    boost::optional<StructExternalSplit>      i_esplopt;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podOptEmpty);
      iar(i_podopt);
      iar(i_iseropt);
      iar(i_isplopt);
      iar(i_eseropt);
      iar(i_esplopt);
    }

    BOOST_CHECK_EQUAL( i_podOptEmpty, o_podOptEmpty);
    BOOST_CHECK(!i_podOptEmpty); // unitialized optionals should evaluate to false
    BOOST_CHECK_EQUAL( i_podopt,    o_podopt );
    BOOST_CHECK_EQUAL( i_iseropt, o_iseropt );
    BOOST_CHECK_EQUAL( i_isplopt, o_isplopt );
    BOOST_CHECK_EQUAL( i_eseropt, o_eseropt );
    BOOST_CHECK_EQUAL( i_esplopt, o_esplopt );
  }
}

BOOST_AUTO_TEST_CASE( binary_opt )
{
  test_opt<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_opt )
{
  test_opt<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_opt )
{
  test_opt<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}
BOOST_AUTO_TEST_CASE( json_opt )
{
  test_opt<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

