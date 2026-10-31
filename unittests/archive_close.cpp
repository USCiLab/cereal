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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "common.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <sstream>

template <class IArchive>
bool can_load_value(std::string const & data, int expected)
{
  try
  {
    std::istringstream is(data);
    IArchive iar(is);
    int value = 0;
    iar(cereal::make_nvp("value", value));
    return value == expected;
  }
  catch (...)
  {
    return false;
  }
}

template <class IArchive, class OArchive>
void test_archive_close_and_reuse()
{
  std::ostringstream os;

  // 1. Calling close() flushes output before destructor runs
  {
    OArchive oar(os);
    oar(cereal::make_nvp("value", 42));

    // Before close, document is incomplete and cannot be loaded
    CHECK_FALSE(can_load_value<IArchive>(os.str(), 42));

    oar.close();
    std::string const flushed = os.str();

    // After close, document is complete and can be loaded
    CHECK(can_load_value<IArchive>(flushed, 42));

    // 2. Calling close() again is an idempotent no-op
    CHECK_NOTHROW(oar.close());
    CHECK(os.str() == flushed);

    // 3. Reusing after close() is a safe no-op that does not corrupt output or state
    CHECK_NOTHROW(oar(cereal::make_nvp("extra", 100)));
    CHECK_NOTHROW(oar.saveBinaryValue("test", 4, "binary"));
    CHECK(os.str() == flushed);
  }

  // 4. Verify output remains valid and loadable after archive destruction
  CHECK(can_load_value<IArchive>(os.str(), 42));

  // 5. Calling close() on an empty archive is a safe no-op
  {
    std::ostringstream empty_os;
    {
      OArchive oar(empty_os);
      CHECK_NOTHROW(oar.close());
      CHECK_NOTHROW(oar(cereal::make_nvp("ignored", 123)));
    }
  }
}

TEST_SUITE_BEGIN("archive_close");

TEST_CASE("json_archive_close_and_reuse")
{
  test_archive_close_and_reuse<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

TEST_CASE("xml_archive_close_and_reuse")
{
  test_archive_close_and_reuse<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

TEST_SUITE_END();
