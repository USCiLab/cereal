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
#ifndef CEREAL_TEST_MEMORY_H_
#define CEREAL_TEST_MEMORY_H_
#include "common.hpp"

template <class IArchive, class OArchive> inline
void test_memory()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::shared_ptr<int> o_xptr1 = std::make_shared<int>(random_value<int>(gen));
    std::shared_ptr<int> o_xptr2 = o_xptr1;
    std::shared_ptr<const int> o_xptr3 = o_xptr1;
    std::shared_ptr<int> o_yptr1 = std::make_shared<int>(random_value<int>(gen));
    std::shared_ptr<int> o_yptr2 = o_yptr1;
    std::shared_ptr<int> o_nullptr1;
    std::shared_ptr<int> o_nullptr2;

    std::unique_ptr<int> o_zptr1(new int(random_value<int>(gen)));
    std::unique_ptr<const int> o_zptr2(new int(random_value<int>(gen)));
    std::unique_ptr<int> o_nullptr3;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_xptr1, o_xptr2, o_xptr3 );
      oar( o_yptr1, o_yptr2 );
      oar( o_nullptr1, o_nullptr2 );

      oar( o_zptr1, o_zptr2 );
      oar( o_nullptr3 );
    }

    std::shared_ptr<int> i_xptr1;
    std::shared_ptr<int> i_xptr2;
    std::shared_ptr<const int> i_xptr3;
    std::shared_ptr<int> i_yptr1;
    std::shared_ptr<int> i_yptr2;
    std::shared_ptr<int> i_nullptr1;
    std::shared_ptr<int> i_nullptr2;

    std::unique_ptr<int> i_zptr1;
    std::unique_ptr<const int> i_zptr2;
    std::unique_ptr<int> i_nullptr3;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_xptr1, i_xptr2, i_xptr3 );
      iar( i_yptr1, i_yptr2 );
      iar( i_nullptr1, i_nullptr2 );

      iar( i_zptr1, i_zptr2 );
      iar( i_nullptr3 );
    }

    CHECK_EQ(o_xptr1.get(), o_xptr2.get());
    CHECK_EQ(o_xptr1.get(), o_xptr3.get());
    CHECK_EQ(i_xptr1.get(), i_xptr2.get());
    CHECK_EQ(i_xptr1.get(), i_xptr3.get());
    CHECK_EQ(*i_xptr1,      *i_xptr2);
    CHECK_EQ(*i_xptr1,      *i_xptr3);

    CHECK_EQ(o_yptr1.get(), o_yptr2.get());
    CHECK_EQ(i_yptr1.get(), i_yptr2.get());
    CHECK_EQ(*i_yptr1,      *i_yptr2);
    CHECK_UNARY_FALSE(i_nullptr1);
    CHECK_UNARY_FALSE(i_nullptr2);

    CHECK_EQ(*i_xptr1, *o_xptr1);
    CHECK_EQ(*i_xptr2, *o_xptr2);
    CHECK_EQ(*i_xptr3, *o_xptr3);

    CHECK_EQ(*i_zptr1, *o_zptr1);
    CHECK_EQ(*i_zptr2, *o_zptr2);
    CHECK_UNARY_FALSE(i_nullptr3);
  }
}

class TestClass
{
  public:
    TestClass(int v) : x(v) { }
    int x;

  private:
    friend class cereal::access;
    TestClass() = default;

    template<class Archive>
      void serialize(Archive & ar) { ar(x); }
};

template <class IArchive, class OArchive> inline
void test_default_construction()
{
  auto o_ptr = std::make_shared<TestClass>(1);
  auto o_ptr2 = std::make_shared<const TestClass>(1);
  std::shared_ptr<TestClass> i_ptr;
  std::shared_ptr<const TestClass> i_ptr2;

  std::ostringstream os;
  {
    OArchive oar(os);
    oar(o_ptr);
    oar(o_ptr2);
  }
  {
    std::istringstream is(os.str());
    IArchive iar(is);
    iar(i_ptr);
    iar(i_ptr2);
  }
  CHECK_EQ(o_ptr->x, i_ptr->x);
  CHECK_EQ(o_ptr2->x, i_ptr2->x);
}

#endif // CEREAL_TEST_LOAD_CONSTRUCT_H_
