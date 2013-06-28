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
#ifndef CEREAL_DETAILS_STATIC_OBJECT_HPP_
#define CEREAL_DETAILS_STATIC_OBJECT_HPP_

#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

namespace cereal
{
  inline std::string demangle(std::string mangledName)
  {
    int status = 0;
    char *demangledName = NULL;
    std::size_t len;

    demangledName = abi::__cxa_demangle(mangledName.c_str(), 0, &len, &status);

    std::string retName(demangledName);
    free(demangledName);

    return retName;
  }

  template<class T> inline
  std::string demangledName()
  { return demangle(typeid(T).name()); }
}

namespace cereal
{
  namespace detail
  {
    //! A static, pre-execution object
    /*! This class will create a single copy (singleton) of some
        type and ensures that merely referencing this type will
        cause it to be instantiated and initialized pre-execution.

        For example, this is used heavily in the polymorphic pointer
        serialization mechanisms to bind various archive types with
        different polymorphic classes */
    template <class T>
    class StaticObject
    {
      private:
        //! Forces instantiation at pre-execution time
        static void instantiate(T const &) {}

        static T & create()
        {
          static T t;
          std::cout << demangledName<T>() << std::endl;
          instantiate(instance);
          return t;
        }

        StaticObject( StaticObject const & other ) = delete;

      public:
        static T & getInstance()
        {
          return create();
        }

      private:
        static T & instance;
    };

    template <class T> T & StaticObject<T>::instance = StaticObject<T>::create();
  }
} // namespace cereal

#endif // CEREAL_DETAILS_STATIC_OBJECT_HPP_
