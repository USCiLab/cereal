/*
  Copyright (c) 2015, Kyle Fleming
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
#ifndef CEREAL_TEST_BOOST_VARIANT_H_
#define CEREAL_TEST_BOOST_VARIANT_H_

#include "../common.hpp"
#include <cereal/types/boost_variant.hpp>
#include <boost/variant.hpp>

struct NonDefaultConstructible
{
    NonDefaultConstructible(int i) : index(i)
    {}

    template <class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME( Archive & ar )
    {
        ar( index );
    }
    int index;
};

namespace cereal
{
template <> struct LoadAndConstruct<NonDefaultConstructible>
{
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<NonDefaultConstructible> & construct )
    {
        int i;
        ar( i );
        construct( i );
    }
};
} // end namespace cereal

template <class IArchive, class OArchive> inline
void test_boost_variant()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  using VariantType = boost::variant<int, double, std::string, NonDefaultConstructible>;

  VariantType o_bv1 = random_value<int>(gen);
  VariantType o_bv2 = random_value<double>(gen);
  VariantType o_bv3 = random_basic_string<char>(gen);
  VariantType o_bv4 = NonDefaultConstructible(random_value<int>(gen));

  std::ostringstream os;
  {
    OArchive oar(os);

    oar(o_bv1);
    oar(o_bv2);
    oar(o_bv3);
    oar(o_bv4);
  }

  decltype(o_bv1) i_bv1;
  decltype(o_bv2) i_bv2;
  decltype(o_bv3) i_bv3;
  decltype(o_bv4) i_bv4;

  std::istringstream is(os.str());
  {
    IArchive iar(is);

    iar(i_bv1);
    iar(i_bv2);
    iar(i_bv3);
    iar(i_bv4);
  }

  CHECK_EQ( boost::get<int>(i_bv1), boost::get<int>(o_bv1) );
  CHECK_EQ( boost::get<double>(i_bv2), doctest::Approx(boost::get<double>(o_bv2)).epsilon(1e-5) );
  CHECK_EQ( boost::get<std::string>(i_bv3), boost::get<std::string>(o_bv3) );
  CHECK_EQ( boost::get<NonDefaultConstructible>(i_bv4).index, boost::get<NonDefaultConstructible>(o_bv4).index );
}

#endif // CEREAL_TEST_BOOST_VARIANT_H_
