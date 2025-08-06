/*
  Copyright (c) 2025, Francois Michaut
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

#ifndef CEREAL_TEST_CPP17_FILESYSTEM_H_
#define CEREAL_TEST_CPP17_FILESYSTEM_H_
#include "../common.hpp"

#ifdef CEREAL_HAS_CPP17

#include <cereal/types/filesystem.hpp>

template <class IArchive, class OArchive> inline
void test_std_filesystem_path()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  using char_type = std::filesystem::path::value_type;

  for(size_t i=0; i<100; ++i)
  {

    std::filesystem::path path_a = random_basic_string<char_type>(gen);
    std::filesystem::path path_b = random_basic_string<char_type>(gen);
    std::filesystem::path path_c = random_basic_string<char_type>(gen);

    std::filesystem::path o_path1 = random_basic_string<char_type>(gen);
    std::filesystem::path o_path2 = "";
    std::filesystem::path o_path3;
    std::filesystem::path o_long_path = path_a / path_b / path_c;

    std::ostringstream os;
    {
      OArchive oar(os);
      oar(o_path1);
      oar(o_path2);
      oar(o_path3);
      oar(o_long_path);
    }

    std::filesystem::path i_path1;
    std::filesystem::path i_path2;
    std::filesystem::path i_path3;
    std::filesystem::path i_long_path;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_path1);
      iar(i_path2);
      iar(i_path3);
      iar(i_long_path);
    }

    CHECK_EQ(i_path1, o_path1);
    CHECK_EQ(i_path2, o_path2);
    CHECK_EQ(i_path3, o_path3);
    CHECK_EQ(i_long_path, o_long_path);
  }
}

#endif // CEREAL_HAS_CPP17
#endif // CEREAL_TEST_CPP17_VARIANT_H_
