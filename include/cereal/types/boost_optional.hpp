/*! \file boost_optional.hpp
\brief Support for boost::optional
\ingroup OtherTypes */
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
#ifndef CEREAL_TYPES_BOOST_OPTIONAL_HPP_
#define CEREAL_TYPES_BOOST_OPTIONAL_HPP_

#include <cereal/cereal.hpp>
#include <boost/optional.hpp>

namespace cereal
{
   //! Saving for boost::optional
   template <class Archive, class Optioned> inline
      void save(Archive & ar, boost::optional<Optioned> const & optional)
   {
      bool initFlag = (bool)optional;
      if (initFlag)
      {
         ar(_CEREAL_NVP("initialized", true));
         ar(_CEREAL_NVP("value", optional.get()));
      }
      else
      {
         ar(_CEREAL_NVP("initialized", false));
      }
   }

   //! Loading for boost::optional
   template <class Archive, class Optioned> inline
      void load(Archive & ar, boost::optional<Optioned> & optional)
   {

      bool initFlag;
      ar(_CEREAL_NVP("initialized", initFlag));
      if (initFlag)
      {
         Optioned val;
         ar(_CEREAL_NVP("value", val));
         optional = val;
      }
      else
         optional = boost::none; // this is all we need to do to reset the internal flag and value

   }
} // namespace cereal

#endif // CEREAL_TYPES_BOOST_OPTIONAL_HPP_
