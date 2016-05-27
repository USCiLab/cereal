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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES AND SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "common.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace
{
struct TestBase : boost::intrusive_ref_counter<TestBase>
{
    explicit TestBase(int i = 0)
        : index(i)
    {
    }

    virtual ~TestBase() = default;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar( CEREAL_NVP(index) );
    }

    int index;
};

struct TestChild final : TestBase
{
    explicit TestChild(std::string k = "", int i = 0)
        : TestBase(i)
        , key(std::move(k))
    {
    }

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<TestBase>(this));
        ar( CEREAL_NVP(key) );
    }

    std::string key;
};

template <class IArchive, class OArchive>
void test_intrusive_ptr()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    auto o_xptr1 = boost::intrusive_ptr<TestChild>(new TestChild(random_value<std::string>(gen),
                                                                                 random_value<int>(gen)));
    auto o_xptr2 = o_xptr1;
    boost::intrusive_ptr<TestBase> o_xptr3 = o_xptr1;
    auto o_yptr1 = boost::intrusive_ptr<TestChild>(new TestChild(random_value<std::string>(gen),
                                                                 random_value<int>(gen)));
    auto o_yptr2 = o_yptr1;
    boost::intrusive_ptr<TestChild> o_nullptr;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_xptr1 );
      oar( o_xptr2 );
      oar( o_xptr3 );
      oar( o_yptr1 );
      oar( o_yptr2 );
      oar( o_nullptr );
    }

    boost::intrusive_ptr<TestChild> i_xptr1;
    boost::intrusive_ptr<TestChild> i_xptr2;
    boost::intrusive_ptr<TestBase> i_xptr3;
    boost::intrusive_ptr<TestChild> i_yptr1;
    boost::intrusive_ptr<TestChild> i_yptr2;
    boost::intrusive_ptr<TestChild> i_nullptr;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_xptr1 );
      iar( i_xptr2 );
      iar( i_xptr3 );
      iar( i_yptr1 );
      iar( i_yptr2 );
      iar( i_nullptr );
    }

    BOOST_CHECK_EQUAL(o_xptr1.get(), o_xptr2.get());
    BOOST_CHECK_EQUAL(o_xptr1.get(), o_xptr3.get());
    BOOST_CHECK_EQUAL(3U, o_xptr3->use_count());

    BOOST_CHECK_EQUAL(i_xptr1.get(), i_xptr2.get());
    BOOST_CHECK_EQUAL(i_xptr1.get(), i_xptr3.get());
    BOOST_CHECK_EQUAL(3U, i_xptr3->use_count());
    BOOST_CHECK_EQUAL(i_xptr1->key, i_xptr2->key);
    BOOST_CHECK_EQUAL(i_xptr1->index, i_xptr3->index);

    BOOST_CHECK_EQUAL(o_yptr1.get(), o_yptr2.get());
    BOOST_CHECK_EQUAL(i_yptr1.get(), i_yptr2.get());
    BOOST_CHECK_EQUAL(i_yptr1->key, i_yptr2->key);
    BOOST_CHECK_EQUAL(i_yptr1->index, i_yptr2->index);
    BOOST_CHECK(!i_nullptr);
  }
}
} // anonymous namespace

CEREAL_REGISTER_TYPE(TestChild)

BOOST_AUTO_TEST_CASE( binary_intrusive_ptr )
{
  test_intrusive_ptr<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_intrusive_ptr )
{
  test_intrusive_ptr<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_intrusive_ptr )
{
  test_intrusive_ptr<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_intrusive_ptr )
{
  test_intrusive_ptr<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}
