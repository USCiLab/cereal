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

//template <class IArchive, class OArchive>
struct test_boost_dynamic_bitset
{
   template <typename ArchiveSet>
   void operator()(ArchiveSet x)
   {
      BOOST_LOG_TRIVIAL(debug) << "\n...Starting test_boost_dynamic_bitset for " << typeid(ArchiveSet).name();
      std::random_device rd;
      std::mt19937 gen(rd());

      auto rng32 = [&]() { return random_binary_string<32>(gen); };
      auto rng65 = [&]() { return random_binary_string<65>(gen); };
      auto rng256 = [&]() { return random_binary_string<256>(gen); };

      for (int ii = 0; ii < 100; ++ii)
      {
         boost::dynamic_bitset<> o_bit32(rng32());
         boost::dynamic_bitset<> o_bit65(rng65());
         boost::dynamic_bitset<> o_bit256(rng256());

         typename ArchiveSet::ostream os; //  std::ostringstream os;
         {
            typename ArchiveSet::oarchive oar(os); //  OArchive oar(os);

            oar(o_bit32);
            oar(o_bit65);
            oar(o_bit256);
         }

         boost::dynamic_bitset<>  i_bit32;
         boost::dynamic_bitset<>  i_bit65;
         boost::dynamic_bitset<>    i_bit256;

         typename ArchiveSet::istream is(os.str()); //  ::istringstream is(os.str());
         {
            typename ArchiveSet::iarchive iar(is); //  IArchive iar(is);

            iar(i_bit32);
            iar(i_bit65);
            iar(i_bit256);
         }

         BOOST_CHECK_EQUAL(o_bit32, i_bit32);
         BOOST_CHECK_EQUAL(o_bit65, i_bit65);
         BOOST_CHECK_EQUAL(o_bit256, i_bit256);
      }
   }
};

BOOST_AUTO_TEST_CASE(all_archives_boost_dynamic_bitset)
{
   boost::mpl::for_each<archive_type_list>(test_boost_dynamic_bitset());
}


//BOOST_AUTO_TEST_CASE(binary_boost_dynamic_bitset)
//{
//   test_boost_dynamic_bitset<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
//}
//
//BOOST_AUTO_TEST_CASE(portable_binary_boost_dynamic_bitset)
//{
//   test_boost_dynamic_bitset<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
//}
//
//BOOST_AUTO_TEST_CASE(xml_boost_dynamic_bitset)
//{
//   test_boost_dynamic_bitset<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
//}
//
//BOOST_AUTO_TEST_CASE(json_boost_dynamic_bitset)
//{
//   test_boost_dynamic_bitset<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
//}


