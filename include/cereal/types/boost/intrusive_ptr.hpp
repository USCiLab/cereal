/*! \file intrusive_ptr.hpp
    \brief Support for boost::intrusive_ptr
    \ingroup OtherTypes */
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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <boost/intrusive_ptr.hpp>

namespace cereal
{
  //! Saving for boost::intrusive_ptr
  template <class Archive, typename T> inline
  void CEREAL_SAVE_FUNCTION_NAME( Archive & ar, boost::intrusive_ptr<T> const & ptr )
  {
    uint32_t id = ar.registerSharedPointer(ptr.get());
    ar( CEREAL_NVP(id) );

    if (id & detail::msb_32bit)
    {
        std::unique_ptr<T, detail::EmptyDeleter<T>> temp(ptr.get());
        ar(temp);
    }
  }

  //! Loading for boost::intrusive_ptr
  template <class Archive, typename T> inline
  void CEREAL_LOAD_FUNCTION_NAME( Archive & ar, boost::intrusive_ptr<T> & ptr )
  {
    uint32_t id;
    ar( CEREAL_NVP(id) );

    if (id & detail::msb_32bit)
    {
        std::unique_ptr<T> temp;
        ar( temp );
        ptr = temp.release();

        auto sharedPtr = std::make_shared<boost::intrusive_ptr<T>>(ptr);
        ar.registerSharedPointer( id, std::shared_ptr<void>(sharedPtr, ptr.get()) );
    }
    else
    {
        ptr = static_cast<T*>( ar.getSharedPointer(id).get() );
    }
  }
} // namespace cereal
