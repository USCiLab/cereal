#include "common.hpp"

template <class IArchive, class OArchive>
void test_memory()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::shared_ptr<int> o_xptr1 = std::make_shared<int>(random_value<int>(gen));
    std::shared_ptr<int> o_xptr2 = o_xptr1;
    std::shared_ptr<int> o_yptr1 = std::make_shared<int>(random_value<int>(gen));
    std::shared_ptr<int> o_yptr2 = o_yptr1;
    std::shared_ptr<int> o_nullptr1;
    std::shared_ptr<int> o_nullptr2;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_xptr1, o_xptr2 );
      oar( o_yptr1, o_yptr2 );
      oar( o_nullptr1, o_nullptr2 );
    }

    std::shared_ptr<int> i_xptr1;
    std::shared_ptr<int> i_xptr2;
    std::shared_ptr<int> i_yptr1;
    std::shared_ptr<int> i_yptr2;
    std::shared_ptr<int> i_nullptr1;
    std::shared_ptr<int> i_nullptr2;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_xptr1, i_xptr2);
      iar( i_yptr1, i_yptr2);
      iar( i_nullptr1, i_nullptr2 );
    }

    BOOST_CHECK_EQUAL(o_xptr1.get(), o_xptr2.get());
    BOOST_CHECK_EQUAL(i_xptr1.get(), i_xptr2.get());
    BOOST_CHECK_EQUAL(*i_xptr1,      *i_xptr2);

    BOOST_CHECK_EQUAL(o_yptr1.get(), o_yptr2.get());
    BOOST_CHECK_EQUAL(i_yptr1.get(), i_yptr2.get());
    BOOST_CHECK_EQUAL(*i_yptr1,      *i_yptr2);
    BOOST_CHECK(!i_nullptr1);
    BOOST_CHECK(!i_nullptr2);
  }
}

BOOST_AUTO_TEST_CASE( binary_memory )
{
  test_memory<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_memory )
{
  test_memory<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_memory )
{
  test_memory<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_memory )
{
  test_memory<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

