/*! \file filesystem.hpp
    \brief Support for types found in \<filesystem\>
    \ingroup STLSupport */
/*
  Copyright (c) 2023, Francois Michaut
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
#ifndef CEREAL_TYPES_FILESYSTEM_HPP_
#define CEREAL_TYPES_FILESYSTEM_HPP_

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include <filesystem>

namespace cereal
{
  //! Serialization for std::filesystem::path types, if binary data is supported
  template<class Archive> inline
  typename std::enable_if<traits::is_output_serializable<BinaryData<std::filesystem::path::value_type>, Archive>::value, void>::type
  CEREAL_SAVE_FUNCTION_NAME(Archive & ar, std::filesystem::path const & path)
  {
    // Save native path format
    ar(path.native());
  }

  //! Serialization for std::filesystem::path types, if binary data is supported
  template<class Archive> inline
  typename std::enable_if<traits::is_input_serializable<BinaryData<std::filesystem::path::value_type>, Archive>::value, void>::type
  CEREAL_LOAD_FUNCTION_NAME(Archive & ar, std::filesystem::path & path)
  {
    // load native path string
    std::filesystem::path::string_type str;

    ar(str);
    path = std::move(str);
  }

} // namespace cereal

#endif // CEREAL_TYPES_FILESYSTEM_HPP_
