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
void test_chrono()
{
  for(int ii=0; ii<100; ++ii)
  {
    auto o_timePoint1 = std::chrono::system_clock::now();
    #ifndef CEREAL_OLDER_GCC
    auto o_timePoint2 = std::chrono::steady_clock::now();
    #endif // CEREAL_OLDER_GCC
    auto o_timePoint3 = std::chrono::high_resolution_clock::now();

    auto o_duration1 = std::chrono::system_clock::now() - o_timePoint1;
    #ifndef CEREAL_OLDER_GCC
    auto o_duration2 = std::chrono::steady_clock::now() - o_timePoint2;
    #endif // CEREAL_OLDER_GCC
    auto o_duration3 = std::chrono::high_resolution_clock::now() - o_timePoint3;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_timePoint1);
      #ifndef CEREAL_OLDER_GCC
      oar(o_timePoint2);
      #endif // CEREAL_OLDER_GCC
      oar(o_timePoint3);
      oar(o_duration1);
      #ifndef CEREAL_OLDER_GCC
      oar(o_duration2);
      #endif // CEREAL_OLDER_GCC
      oar(o_duration3);
    }

    decltype(o_timePoint1) i_timePoint1;
    #ifndef CEREAL_OLDER_GCC
    decltype(o_timePoint2) i_timePoint2;
    #endif // CEREAL_OLDER_GCC
    decltype(o_timePoint3) i_timePoint3;
    decltype(o_duration1) i_duration1;
    #ifndef CEREAL_OLDER_GCC
    decltype(o_duration2) i_duration2;
    #endif // CEREAL_OLDER_GCC
    decltype(o_duration3) i_duration3;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_timePoint1);
      #ifndef CEREAL_OLDER_GCC
      iar(i_timePoint2);
      #endif // CEREAL_OLDER_GCC
      iar(i_timePoint3);
      iar(i_duration1);
      #ifndef CEREAL_OLDER_GCC
      iar(i_duration2);
      #endif // CEREAL_OLDER_GCC
      iar(i_duration3);
    }

    BOOST_CHECK( o_timePoint1 == i_timePoint1 );
    #ifndef CEREAL_OLDER_GCC
    BOOST_CHECK( o_timePoint2 == i_timePoint2 );
    #endif // CEREAL_OLDER_GCC
    BOOST_CHECK( o_timePoint3 == i_timePoint3 );
    BOOST_CHECK( o_duration1 == i_duration1 );
    #ifndef CEREAL_OLDER_GCC
    BOOST_CHECK( o_duration2 == i_duration2 );
    #endif // CEREAL_OLDER_GCC
    BOOST_CHECK( o_duration3 == i_duration3 );
  }
}

BOOST_AUTO_TEST_CASE( binary_chrono )
{
  test_chrono<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_chrono )
{
  test_chrono<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_chrono )
{
  test_chrono<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_chrono )
{
  test_chrono<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}


