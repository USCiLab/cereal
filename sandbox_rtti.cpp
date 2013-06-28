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

#include <type_traits>
#include <cereal/details/static_object.hpp>

namespace cereal
{
  namespace detail
  {
    struct InputArchiveBase;
    struct OutputArchiveBase;

    //TODO
    template <class Archive, class T> struct InputBinding {};
    template <class Archive, class T> struct OutputBinding {};

    struct adl_tag {};

    //! Causes the static object bindings between an archive type and a serializable type T
    template <class Archive, class T>
    struct create_bindings
    {
        static const InputBinding<Archive, T> &
        load(std::true_type)
        {
          return cereal::detail::StaticObject<InputBinding<Archive, T>>::getInstance();
        }

        static const OutputBinding<Archive, T> &
        save(std::true_type)
        {
          return cereal::detail::StaticObject<OutputBinding<Archive, T>>::getInstance();
        }

        inline static void load(std::false_type) {}
        inline static void save(std::false_type) {}
    };

    //! When specialized, causes the compiler to instantiate its parameter
    template <void(*)()>
    struct instantiate_function {};

    template <class Archive, class T>
    struct polymorphic_serialization_support
    {
      static void instantiate();
      typedef instantiate_function<instantiate> unused;
    };

    template <class Archive, class T>
    void polymorphic_serialization_support<Archive,T>::instantiate()
    {
        create_bindings<Archive,T>::save( std::is_base_of<detail::OutputArchiveBase, Archive>() );

        create_bindings<Archive,T>::load( std::is_base_of<detail::InputArchiveBase, Archive>() );
    }

    namespace detail2
    {
      template <class T>
      struct bind_to_archives
      {
        void bind(std::false_type) const
        {
          instantiate_polymorphic_binding( (T*)0, 0, adl_tag{} );
        }

        void bind(std::true_type) const
        { }

        bind_to_archives const & bind() const
        {
          bind( std::is_abstract<T>() );
          return *this;
        }
      };

      template <class T>
      struct init_binding;
    } // end namespace

    template <class T>
    void instantiate_polymorphic_binding( T*, int, adl_tag ) {};

#define CEREAL_REGISTER_ARCHIVE(Archive)                                \
    namespace cereal { namespace detail {                               \
    template <class T>                                                  \
    typename polymorphic_serialization_support<Archive, T>::type        \
    instantiate_polymorphic_binding( T*, Archive*, adl_tag );           \
    } } // end namespaces

#define CEREAL_BIND_TO_ARCHIVES(T)                           \
    namespace cereal {                                       \
    namespace detail {                                       \
    namespace detail2 {                                      \
    template<>                                               \
    struct init_binding<T> {                                 \
        static bind_to_archives<T> const & b;                \
    };                                                       \
    bind_to_archives<T> const & init_binding<T>::b =         \
        ::cereal::detail::StaticObject<                      \
            bind_to_archives<T >                             \
        >::getInstance().bind();                             \
    }}} // end namespaces
  } // namespace detail
} // namespace cereal

#include <cereal/archives/binary.hpp>

struct MyType {};

CEREAL_REGISTER_ARCHIVE(BinaryInputArchive);
CEREAL_BIND_TO_ARCHIVES(MyType);

template <class T>
void nop(T&&t) {}
int main()
{
  //cereal::Singleton<boost::archive::detail::extra_detail::guid_initializer<MyType>>::getInstance();
}
