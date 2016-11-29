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
#ifndef CEREAL_TEST_BOOST_COMMON_H_
#define CEREAL_TEST_BOOST_COMMON_H_

// Pull in all the standard unit test header info
#include "../common.hpp"

// Group archive types that go together
template <typename outArchiveType, typename inArchiveType, typename outStream, typename inStream>
struct archiveTypes
{
   typedef typename outArchiveType  oarchive;
   typedef typename inArchiveType   iarchive;
   typedef typename outStream       ostream;
   typedef typename inStream        istream;
};

typedef archiveTypes<cereal::BinaryOutputArchive         , cereal::BinaryInputArchive        , std::ostringstream, std::istringstream>    cereal_binary;
typedef archiveTypes<cereal::JSONOutputArchive           , cereal::JSONInputArchive          , std::ostringstream, std::istringstream>    cereal_JSON;
typedef archiveTypes<cereal::PortableBinaryOutputArchive , cereal::PortableBinaryInputArchive, std::ostringstream, std::istringstream>    cereal_portable_binary;
typedef archiveTypes<cereal::XMLOutputArchive            , cereal::XMLInputArchive           , std::ostringstream, std::istringstream>    cereal_XML;

typedef boost::mpl::list<
   cereal_binary
   , cereal_JSON
   , cereal_portable_binary
   , cereal_XML
> archive_type_list;

#include <boost/log/trivial.hpp>    // for logging
#include <boost/mpl/for_each.hpp>   // to loop through the archive sets

#include <cereal/types/boost/chrono.hpp>
#include <cereal/types/boost/circular_buffer.hpp>
#include <cereal/types/boost/compressed_pair.hpp>
#include <cereal/types/boost/dynamic_bitset.hpp>
#include <cereal/types/boost/filesystem.hpp>
#include <cereal/types/boost/optional.hpp>
#include <cereal/types/boost/units.hpp>



#endif // CEREAL_TEST_BOOST_COMMON_H_
