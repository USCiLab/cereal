#include "common.hpp"

template <class IArchive, class OArchive>
void test_string_basic()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    std::basic_string<char> o_string  = random_basic_string<char>(gen);
    std::basic_string<char> o_string2 = "";

    std::ostringstream os;
    {
      OArchive oar(os);
      oar(o_string);
      oar(o_string2);
    }

    std::basic_string<char> i_string;
    std::basic_string<char> i_string2;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_string);
      iar(i_string2);
    }

    BOOST_CHECK_EQUAL(i_string, o_string);
    BOOST_CHECK_EQUAL(i_string2, o_string2);
  }
}

template <class IArchive, class OArchive>
void test_string_all()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    std::basic_string<char> o_string        = random_basic_string<char>(gen);
    std::basic_string<wchar_t> o_wstring    = random_basic_string<wchar_t>(gen);
    std::basic_string<char16_t> o_u16string = random_basic_string<char16_t>(gen);
    std::basic_string<char32_t> o_u32string = random_basic_string<char32_t>(gen);

    std::ostringstream os;
    {
      OArchive oar(os);
      oar(o_string);
      oar(o_wstring);
      oar(o_u16string);
      oar(o_u32string);
    }

    std::basic_string<char> i_string;
    std::basic_string<wchar_t> i_wstring;
    std::basic_string<char16_t> i_u16string;
    std::basic_string<char32_t> i_u32string;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_string);
      iar(i_wstring);
      iar(i_u16string);
      iar(i_u32string);
    }

    BOOST_CHECK_EQUAL(i_string, o_string);
    BOOST_CHECK_EQUAL_COLLECTIONS(i_wstring.begin(),     i_wstring.end(),   o_wstring.begin(),   o_wstring.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_u16string.begin(),  i_u16string.end(),  o_u16string.begin(), o_u16string.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_u32string.begin(),  i_u32string.end(),  o_u32string.begin(), o_u32string.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_string )
{
  test_string_all<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_string )
{
  test_string_all<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_string_basic )
{
  test_string_basic<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_string_basic )
{
  test_string_basic<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

