/*! \file map.hpp
    \brief Support for types found in \<map\>
    \ingroup STLSupport */
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
#ifndef CEREAL_TYPES_MAP_HPP_
#define CEREAL_TYPES_MAP_HPP_

#include "cereal/types/concepts/pair_associative_container.hpp"
#include <map>

namespace cereal
{
  /**
    * std::multimap must not default to the implementation in
    * pair_associative_container.hpp, because it incorrectly
    * handles equivalent keys when loading the map.
    *
    * The cause is the use of emplace_hint.
    * Since it iserts the value before the hint, key order will be flipped
    *
    * Here we are using emplace (if key is equivalent to the previous one),
    * or emplace_hint if keys are different, because this is a faster path
    *
    * Emplace will preserve order of equivalent keys, while where
    * non-equivalent ones are placed doesn't matter.
    **/

  template <class Archive, typename... Args>
  inline void load(Archive &ar, std::multimap<Args...> &map)
  {
    size_type size;
    ar(make_size_tag(size));

    map.clear();

    typename std::multimap<Args...>::key_compare cmp;
    auto hint = map.begin();
    for (size_t i = 0; i < size; ++i)
    {
      typename std::multimap<Args...>::key_type key;
      typename std::multimap<Args...>::mapped_type value;

      ar(make_map_item(key, value));
      if(!cmp(hint->first, key) && !cmp(key, hint->first)) {
        #ifdef CEREAL_OLDER_GCC
        hint = map.insert(std::move(key), std::move(value));
        #else // NOT CEREAL_OLDER_GCC
        hint = map.emplace(std::move(key), std::move(value));
        #endif // NOT CEREAL_OLDER_GCC
      } else {
        #ifdef CEREAL_OLDER_GCC
        hint = map.insert_hint(hint, std::move(key), std::move(value));
        #else // NOT CEREAL_OLDER_GCC
        hint = map.emplace_hint(hint, std::move(key), std::move(value));
        #endif // NOT CEREAL_OLDER_GCC
      }

    }
  }
}

#endif // CEREAL_TYPES_MAP_HPP_
