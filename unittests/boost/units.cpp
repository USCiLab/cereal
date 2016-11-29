/*
Copyright (c) 2016 Steve Hickman
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

//--------------from Boost::Units kitchen sink example


#include <cmath>
#include <complex>
#include <iostream>

#include <boost/typeof/std/complex.hpp>

#include <boost/units/cmath.hpp>
#include <boost/units/io.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/codata/physico-chemical_constants.hpp>
#include <boost/units/systems/si/io.hpp>

//#include "measurement.hpp"

namespace boost {

   namespace units {

      //[kitchen_sink_function_snippet_3
      /// the physical definition of work - computed for an arbitrary unit system 
      template<class System, class Y>
      quantity<unit<energy_dimension, System>, Y>
         work(quantity<unit<force_dimension, System>, Y> F,
            quantity<unit<length_dimension, System>, Y> dx)
      {
         return F*dx;
      }
      //]

      //[kitchen_sink_function_snippet_4
      /// the ideal gas law in si units
      template<class Y>
      quantity<si::amount, Y>
         idealGasLaw(const quantity<si::pressure, Y>& P,
            const quantity<si::volume, Y>& V,
            const quantity<si::temperature, Y>& T)
      {
         using namespace boost::units::si;

         using namespace constants::codata;
         return (P*V / (R*T));
      }
      //]

   } // namespace units

} // namespace boost

//template  <IArchive, class OArchive>
struct test_boost_units
{
   template <typename ArchiveSet>
   void operator()(ArchiveSet x)
   {
      BOOST_LOG_TRIVIAL(debug) << "\n...Starting test_boost_units for " << typeid(ArchiveSet).name();

      using namespace boost::units;
      using namespace boost::units::si;

      std::random_device rd;
      std::mt19937 gen(rd());

      for (size_t i = 0; i < 100; ++i)
      {
         /// scalar
         const double                  o_s1 = random_value<double>(gen);

         const long                    o_x1 = random_value<long>(gen);

         // static_rational serialization not supported - but since its const, it wouldn't matter anyway
         const static_rational<4, 3>   no_x2;

         /// define some units
         force                         o_u1 = newton;
         energy                        o_u2 = joule;

         /// define some quantities
         quantity<force>               o_q1(random_value<float>(gen)*o_u1);
         quantity<energy>              o_q2(random_value<float>(gen)*o_u2);

         quantity<length>              o_l1(random_value<double>(gen)*meter),
            o_l2(random_value<double>(gen)*meters);

         const double                  o_dimless = (o_q1 / o_q1);

         quantity<velocity>            o_v1 = random_value<float>(gen) *meters / second;


         //[kitchen_sink_snippet_4
         /// test calculation of work
         quantity<force>               o_F(random_value<float>(gen)*newton);
         quantity<length>              o_dx(random_value<double>(gen)*meter);
         quantity<energy>              o_E(work(o_F, o_dx));

         typename ArchiveSet::ostream os; //  std::ostringstream os;
         {
            typename ArchiveSet::oarchive oar(os); //  OArchive oar(os);
            oar(o_s1);
            oar(o_x1);
            // static_rational serialization not supported - but since its const, it wouldn't matter anyway
            //oar(no_x2);
            oar(o_u1);
            oar(o_u2);
            oar(o_q1);
            oar(o_q2);
            oar(o_l1);
            oar(o_l2);
            oar(o_dimless);
            oar(o_v1);
            oar(o_F);
            oar(o_dx);
            oar(o_E);
         }

         double                        i_s1 = 0;

         long                          i_x1 = 0;
         //static_rational<4, 3>         i_x2;

         /// define some units
         force                         i_u1;
         energy                        i_u2;

         /// define some quantities
         quantity<force>               i_q1;
         quantity<energy>              i_q2;

         quantity<length>              i_l1,
            i_l2;

         double                        i_dimless;

         quantity<velocity>            i_v1;


         //[kitchen_sink_snippet_4
         /// test calculation of work
         quantity<force>               i_F;
         quantity<length>              i_dx;
         quantity<energy>              i_E;


         typename ArchiveSet::istream is(os.str()); //  ::istringstream is(os.str());
         {
            typename ArchiveSet::iarchive iar(is); //  IArchive iar(is);
            iar(i_s1);
            iar(i_x1);
            // static_rational serialization not supported - but since its const, it wouldn't matter anyway
            //iar(i_x2);
            iar(i_u1);
            iar(i_u2);
            iar(i_q1);
            iar(i_q2);
            iar(i_l1);
            iar(i_l2);
            iar(i_dimless);
            iar(i_v1);
            iar(i_F);
            iar(i_dx);
            iar(i_E);
         }

         BOOST_CHECK_CLOSE(o_s1, i_s1, 1e-5);
         BOOST_CHECK_EQUAL(o_x1, i_x1);
         //BOOST_CHECK_EQUAL(o_x2, i_x2);
         BOOST_CHECK_EQUAL(o_u1, i_u1);
         BOOST_CHECK_EQUAL(o_u2, i_u2);
         BOOST_CHECK_CLOSE(o_q1.value(), i_q1.value(), 1e-5);
         BOOST_CHECK_CLOSE(o_q2.value(), i_q2.value(), 1e-5);
         BOOST_CHECK_CLOSE(o_l1.value(), i_l1.value(), 1e-5);
         BOOST_CHECK_CLOSE(o_l2.value(), i_l2.value(), 1e-5);
         BOOST_CHECK_CLOSE(o_dimless, i_dimless, 1e-5);
         BOOST_CHECK_CLOSE(o_v1.value(), i_v1.value(), 1e-5);
         BOOST_CHECK_CLOSE(o_F.value(), i_F.value(), 1e-5);

         quantity<length> dx_diff = 1e-5 * meter;
         BOOST_CHECK_CLOSE(o_dx.value(), i_dx.value(), 1e-5); //, dx_diff);
         quantity<energy>  work_diff = (float)1e-5 * newton * meter;
         BOOST_CHECK_CLOSE(o_E.value(), i_E.value(), 1e-5); // , work_diff);

      }
   }
};

BOOST_AUTO_TEST_CASE(all_archives_boost_units)
{
   boost::mpl::for_each<archive_type_list>(test_boost_units());
}
//
//BOOST_AUTO_TEST_CASE(xml_boost_units)
//{
//   test_boost_units<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
//}
//
//BOOST_AUTO_TEST_CASE(json_boost_units)
//{
//   test_boost_units<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
//}
//
//BOOST_AUTO_TEST_CASE(binary_boost_units)
//{
//   test_boost_units<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
//}
//
//BOOST_AUTO_TEST_CASE(portable_binary_boost_units)
//{
//   test_boost_units<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
//}