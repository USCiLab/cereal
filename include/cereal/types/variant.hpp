/*! \file variant.hpp
    \brief Support for std::variant
    \ingroup STLSupport */
/*
  Copyright (c) 2014, 2017, Randolph Voorhies, Shane Grant, Juan Pedro
  Bolivar Puente. All rights reserved.

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
#ifndef CEREAL_TYPES_STD_VARIANT_HPP_
#define CEREAL_TYPES_STD_VARIANT_HPP_

#include "cereal/cereal.hpp"
#include <variant>
#include <cstdint>
#ifdef CEREAL_NAMED_VARIANT
#include <map>
#endif

namespace cereal
{
  namespace variant_detail
  {
    //! @internal
    template <class Archive>
    struct variant_save_visitor
    {
      variant_save_visitor(Archive & ar_, const std::string & n) : ar(ar_), name(n) {}

      template<class T>
        void operator()(T const & value) const
        {
          ar( CEREAL_NVP_(name.c_str(), value) );
        }

      Archive & ar;
      std::string name;
    };

    //! @internal
    template<int N, class Variant, class ... Args, class Archive>
    typename std::enable_if<N == std::variant_size_v<Variant>, void>::type
    load_variant(Archive & /*ar*/, int /*target*/, const std::string & /*name*/, Variant & /*variant*/)
    {
      throw ::cereal::Exception("Error traversing variant during load");
    }
    //! @internal
    template<int N, class Variant, class H, class ... T, class Archive>
    typename std::enable_if<N < std::variant_size_v<Variant>, void>::type
    load_variant(Archive & ar, int target, const std::string & name, Variant & variant)
    {
      if(N == target)
      {
        H value;
        ar( CEREAL_NVP_(name.c_str(), value) );
        variant = std::move(value);
      }
      else
        load_variant<N+1, Variant, T...>(ar, target, name, variant);
    }

  } // namespace variant_detail

  //! Saving for std::variant
  template <class Archive, typename VariantType1, typename... VariantTypes> inline
  void CEREAL_SAVE_FUNCTION_NAME( Archive & ar, std::variant<VariantType1, VariantTypes...> const & variant )
  {
#ifdef CEREAL_NAMED_VARIANT
    static const std::vector<std::string> names = {abi::__cxa_demangle(typeid(VariantType1).name(), 0, 0, nullptr),
        std::forward<std::string>(abi::__cxa_demangle(typeid(VariantTypes).name(), 0, 0, nullptr))...};
#endif

    std::int32_t index = static_cast<std::int32_t>(variant.index());

#ifdef CEREAL_NAMED_VARIANT
    if (index >= names.size())
        throw ::cereal::Exception("Error variant type not found");
    ar( CEREAL_NVP_("type", names[index]) );
    std::string container_name = names[index];
#else
    ar( CEREAL_NVP_("index", index) );
    std::string container_name = "data";
#endif

    variant_detail::variant_save_visitor<Archive> visitor(ar, container_name);
    std::visit(visitor, variant);
  }

  //! Loading for std::variant
  template <class Archive, typename... VariantTypes> inline
  void CEREAL_LOAD_FUNCTION_NAME( Archive & ar, std::variant<VariantTypes...> & variant )
  {
    using variant_t = typename std::variant<VariantTypes...>;

#ifdef CEREAL_NAMED_VARIANT
    static const std::map<std::string, int> variant_t_names = {
        std::make_pair<std::string, int>(
            std::forward<std::string>(abi::__cxa_demangle(typeid(VariantTypes).name(), 0, 0, nullptr)),
            variant_t(VariantTypes()).index()) ...
    };
#endif

    std::int32_t index;
    std::string data_key;

#ifdef CEREAL_NAMED_VARIANT
    ar( CEREAL_NVP_("type", data_key) );
    const auto& it = variant_t_names.find(data_key);
    if (it == variant_t_names.end())
        throw Exception("Invalid variant type not found");
    index = it->second;
#else
    ar( CEREAL_NVP_("index", index) );
    data_key = "data";
#endif

    if(index >= static_cast<std::int32_t>(std::variant_size_v<variant_t>))
      throw Exception("Invalid 'index' selector when deserializing std::variant");

    variant_detail::load_variant<0, variant_t, VariantTypes...>(ar, index, data_key, variant);
  }

  //! Serializing a std::monostate
  template <class Archive>
  void CEREAL_SERIALIZE_FUNCTION_NAME( Archive &, std::monostate const & ) {}
} // namespace cereal

#endif // CEREAL_TYPES_STD_VARIANT_HPP_
