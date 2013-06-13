#include <cereal/binary_archive/binary_archive.hpp>
#include <limits>
#include <random>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>
 

template<class T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
random_value(std::mt19937 & gen)
{ return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen); }

template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
random_value(std::mt19937 & gen)
{ return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen); }

BOOST_AUTO_TEST_CASE( pod )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {

    uint8_t  const o_uint8  = random_value<uint8_t>(gen);
    int8_t   const o_int8   = random_value<int8_t>(gen);
    uint16_t const o_uint16 = random_value<uint16_t>(gen);
    int16_t  const o_int16  = random_value<int16_t>(gen);
    uint32_t const o_uint32 = random_value<uint32_t>(gen);
    int32_t  const o_int32  = random_value<int32_t>(gen);
    uint64_t const o_uint64 = random_value<uint64_t>(gen);
    int64_t  const o_int64  = random_value<int64_t>(gen);
    float    const o_float  = random_value<float>(gen);
    double   const o_double = random_value<double>(gen);

    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);
    oar & o_uint8;
    oar & o_int8;
    oar & o_uint16;
    oar & o_int16;
    oar & o_uint32;
    oar & o_int32;
    oar & o_uint64;
    oar & o_int64;
    oar & o_float;
    oar & o_double;

    uint8_t  i_uint8  = 0.0;
    int8_t   i_int8   = 0.0;
    uint16_t i_uint16 = 0.0;
    int16_t  i_int16  = 0.0;
    uint32_t i_uint32 = 0.0;
    int32_t  i_int32  = 0.0;
    uint64_t i_uint64 = 0.0;
    int64_t  i_int64  = 0.0;
    float    i_float  = 0.0;
    double   i_double = 0.0;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);
    iar & i_uint8;
    iar & i_int8;
    iar & i_uint16;
    iar & i_int16;
    iar & i_uint32;
    iar & i_int32;
    iar & i_uint64;
    iar & i_int64;
    iar & i_float;
    iar & i_double;

    BOOST_CHECK_EQUAL(i_uint8  , o_uint8);
    BOOST_CHECK_EQUAL(i_int8   , o_int8);
    BOOST_CHECK_EQUAL(i_uint16 , o_uint16);
    BOOST_CHECK_EQUAL(i_int16  , o_int16);
    BOOST_CHECK_EQUAL(i_uint32 , o_uint32);
    BOOST_CHECK_EQUAL(i_int32  , o_int32);
    BOOST_CHECK_EQUAL(i_uint64 , o_uint64);
    BOOST_CHECK_EQUAL(i_int64  , o_int64);
    BOOST_CHECK_EQUAL(i_float  , o_float);
    BOOST_CHECK_EQUAL(i_double , o_double);
  }
  
}
