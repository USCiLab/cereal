/*! \file macros.hpp
    \brief Preprocessor macros that can customise the cereal library */
/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
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
#ifndef CEREAL_MACROS_HPP_
#define CEREAL_MACROS_HPP_

#ifndef CEREAL_SERIALIZE_FUNCTION_NAME
//! The serialization/deserialization function name to search for.
/*! You can define @c CEREAL_SERIALIZE_FUNCTION_NAME to be different assuming
    you do so before this file is included. */
#define CEREAL_SERIALIZE_FUNCTION_NAME serialize
#endif // CEREAL_SERIALIZE_FUNCTION_NAME

#ifndef CEREAL_LOAD_FUNCTION_NAME
//! The deserialization function name to search for.
/*! You can define @c CEREAL_LOAD_FUNCTION_NAME to be different assuming you do so
    before this file is included. */
#define CEREAL_LOAD_FUNCTION_NAME load
#endif // CEREAL_LOAD_FUNCTION_NAME

#ifndef CEREAL_SAVE_FUNCTION_NAME
//! The serialization function name to search for.
/*! You can define @c CEREAL_SAVE_FUNCTION_NAME to be different assuming you do so
    before this file is included. */
#define CEREAL_SAVE_FUNCTION_NAME save
#endif // CEREAL_SAVE_FUNCTION_NAME

#endif // CEREAL_MACROS_HPP_
