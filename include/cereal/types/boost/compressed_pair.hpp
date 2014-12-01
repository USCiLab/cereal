/*! \file boost_compressed_pair.hpp
    \brief Support for types found in \<boost\compressed_pair.hpp\>
    \ingroup OtherTypes */
/*
  Copyright (c) 2014, Steve HIckman
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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_BOOST_COMPRESSED_PAIR_HPP_
#define CEREAL_TYPES_BOOST_COMPRESSED_PAIR_HPP_

#include <cereal/cereal.hpp>
#include <boost/compressed_pair.hpp>

namespace cereal
{
  //! Saving for boost::compressed_pair
  template <class Archive, class T1, class T2> inline
  void save( Archive & ar, ::boost::compressed_pair<T1, T2> const & pair )
  {
    ar( _CEREAL_NVP("first",  pair.first()),
        _CEREAL_NVP("second", pair.second()) );
  }
  

   //! Loading for boost::optional
  template <class Archive, class T1, class T2> inline
  void load( Archive & ar, ::boost::compressed_pair<T1, T2> & pair )
   {

	T1 first;
	T2 second;

    ar(_CEREAL_NVP("first", first));
    ar(_CEREAL_NVP("second", second));
 
	pair = ::boost::compressed_pair<T1,T2>(first,second);
   }
} // namespace cereal

#endif // CEREAL_TYPES_BOOST_COMPRESSED_PAIR_HPP_
