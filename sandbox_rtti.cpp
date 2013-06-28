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
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sstream>

struct Base 
{
  virtual void foo() = 0;

  template<class Archive>
    void serialize(Archive & ar)
    {
      std::cout << "Serializing Base" << std::endl;
    }
};

struct MyType : public Base
{
  void foo() {}

  template<class Archive>
    void serialize(Archive & ar)
    {
      std::cout << "Serializing MyType" << std::endl;
    }
};

CEREAL_BIND_TO_ARCHIVES(MyType);

template <class T> void nop(T&&t) {}

int main()
{
  std::stringstream stream;
  cereal::BinaryOutputArchive archive(stream);

  std::shared_ptr<Base> ptr = std::make_shared<MyType>();
  archive(ptr);

  std::unique_ptr<int> xxx(nullptr);
  archive(xxx);
}
