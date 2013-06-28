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
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_POLYMORPHIC_HPP_
#define CEREAL_TYPES_POLYMORPHIC_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/details/polymorphic_impl.hpp>

//! Binds a polymorhic type to all registered archives
/*! This binds a polymorphic type to all registered archives that
    have been registered with CEREAL_REGISTER_ARCHIVE.  This must be called
    after all archives are registered (usually after the archives themselves
    have been included). */
#define CEREAL_BIND_TO_ARCHIVES(T)                           \
    namespace cereal {                                       \
    namespace detail {                                       \
    template<>                                               \
    struct init_binding<T> {                                 \
        static bind_to_archives<T> const & b;                \
    };                                                       \
    bind_to_archives<T> const & init_binding<T>::b =         \
        ::cereal::detail::StaticObject<                      \
            bind_to_archives<T >                             \
        >::getInstance().bind();                             \
    }} // end namespaces

namespace cereal
{
  //! Saving std::shared_ptr for polymorphic types
  template <class Archive, class T> inline
  typename std::enable_if<std::is_polymorphic<T>::value, void>::type
  save( Archive & ar, std::shared_ptr<T> const & ptr )
  {
    auto & bindingMap = detail::StaticObject<detail::OutputBindingMap<Archive>>::getInstance().map;

    auto binding = bindingMap.find(std::type_index(typeid(*ptr.get())));
    if(binding == bindingMap.end())
      throw cereal::Exception("Trying to serialize unregistered polymorphic type");

    binding->second.shared_ptr(&ar, ptr.get());
  }

  //! Loading std::shared_ptr, case when user load and allocate for polymorphic types
  template <class Archive, class T> inline
  typename std::enable_if<std::is_polymorphic<T>::value, void>::type
  load( Archive & ar, std::shared_ptr<T> & ptr )
  {
  }

  //! Saving std::weak_ptr for polymorphic types
  template <class Archive, class T> inline
  typename std::enable_if<std::is_polymorphic<T>::value, void>::type
  save( Archive & ar, std::weak_ptr<T> const & ptr )
  {
  }

  //! Loading std::weak_ptr for polymorphic types
  template <class Archive, class T> inline
  typename std::enable_if<std::is_polymorphic<T>::value, void>::type
  load( Archive & ar, std::weak_ptr<T> & ptr )
  {
  }

  //! Saving std::unique_ptr for polymorphic types
  template <class Archive, class T, class D> inline
  typename std::enable_if<std::is_polymorphic<T>::value, void>::type
  save( Archive & ar, std::unique_ptr<T, D> const & ptr )
  {
  }

  //! Loading std::unique_ptr, case when user provides load_and_allocate for polymorphic types
  template <class Archive, class T, class D> inline
  typename std::enable_if<std::is_polymorphic<T>::value, void>::type
  load( Archive & ar, std::unique_ptr<T, D> & ptr )
  {
  }
} // namespace cereal
#endif // CEREAL_TYPES_POLYMORPHIC_HPP_
