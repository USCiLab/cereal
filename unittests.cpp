#include <cereal/binary_archive/binary_archive.hpp>
#include <cereal/binary_archive/memory.hpp>
#include <cereal/binary_archive/array.hpp>
#include <limits>
#include <random>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Cereal
#include <boost/test/unit_test.hpp>

struct StructBase
{
  int x, y;
  bool operator==(StructBase const & other)
  { return x == other.x && y == other.y; }
};
 
struct StructInternalSerialize : StructBase
{
  StructInternalSerialize() : StructBase{0,0} {}
  StructInternalSerialize(int x_, int y_) : StructBase{x_,y_} {} 
  template<class Archive>
    void serialize(Archive & ar)
    {
      ar & x & y;
    }
};

struct StructInternalSplit : StructBase
{
  StructInternalSplit() : StructBase{0,0} {}
  StructInternalSplit(int x_, int y_) : StructBase{x_,y_} {} 
  template<class Archive>
    void save(Archive & ar)
    {
      ar & x & y;
    }

  template<class Archive>
    void load(Archive & ar)
    {
      ar & x & y;
    }
};

struct StructExternalSerialize : StructBase
{
  StructExternalSerialize() : StructBase{0,0} {}
  StructExternalSerialize(int x_, int y_) : StructBase{x_,y_} {} 
};

  template<class Archive>
void serialize(Archive & ar, StructExternalSerialize & s)
{
  ar & s.x & s.y;
}

struct StructExternalSplit : StructBase
{
  StructExternalSplit() : StructBase{0,0} {}
  StructExternalSplit(int x_, int y_) : StructBase{x_,y_} {} 
};

  template<class Archive>
void save(Archive & ar, StructExternalSplit & s)
{
  ar & s.x & s.y;
}

  template<class Archive>
void load(Archive & ar, StructExternalSplit & s)
{
  ar & s.x & s.y;
}

template<class T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
random_value(std::mt19937 & gen)
{ return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen); }

template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
random_value(std::mt19937 & gen)
{ return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen); }

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_pod )
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

// ######################################################################
BOOST_AUTO_TEST_CASE( structs )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    StructInternalSerialize o_iser = { random_value<int>(gen), random_value<int>(gen) };
    StructInternalSplit     o_ispl = { random_value<int>(gen), random_value<int>(gen) };
    StructExternalSerialize o_eser = { random_value<int>(gen), random_value<int>(gen) };
    StructExternalSplit     o_espl = { random_value<int>(gen), random_value<int>(gen) };

    oar & o_iser & o_ispl & o_eser & o_espl;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    StructInternalSerialize i_iser;
    StructInternalSplit     i_ispl;
    StructExternalSerialize i_eser;
    StructExternalSplit     i_espl;

    iar & i_iser & i_ispl & i_eser & i_espl;
  
    BOOST_CHECK(i_iser == o_iser);
    BOOST_CHECK(i_ispl == o_ispl);
    BOOST_CHECK(i_eser == o_eser);
    BOOST_CHECK(i_espl == o_espl);
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_memory )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::shared_ptr<int> o_xptr1 = std::make_shared<int>(random_value<int>(gen));
    std::shared_ptr<int> o_xptr2 = o_xptr1;
    std::shared_ptr<int> o_yptr1 = std::make_shared<int>(random_value<int>(gen));
    std::shared_ptr<int> o_yptr2 = o_yptr1;
    oar & o_xptr1 & o_xptr2;
    oar & o_yptr1 & o_yptr2;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::shared_ptr<int> i_xptr1;
    std::shared_ptr<int> i_xptr2;
    std::shared_ptr<int> i_yptr1;
    std::shared_ptr<int> i_yptr2;
    iar & i_xptr1 & i_xptr2;
    iar & i_yptr1 & i_yptr2;

    BOOST_CHECK_EQUAL(o_xptr1.get(), o_xptr2.get());
    BOOST_CHECK_EQUAL(i_xptr1.get(), i_xptr2.get());
    BOOST_CHECK_EQUAL(*i_xptr1, *i_xptr2);

    BOOST_CHECK_EQUAL(o_yptr1.get(), o_yptr2.get());
    BOOST_CHECK_EQUAL(i_yptr1.get(), i_yptr2.get());
    BOOST_CHECK_EQUAL(*i_yptr1, *i_yptr2);
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_array )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::array<int, 100> o_podarray;
    for(auto & elem : o_podarray)
      elem = random_value<decltype(o_podarray)::value_type>(gen);

    std::array<StructInternalSerialize, 100> o_iserarray;
    for(auto & elem : o_iserarray)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::array<StructInternalSplit, 100> o_isplarray;
    for(auto & elem : o_isplarray)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::array<StructExternalSerialize, 100> o_eserarray;
    for(auto & elem : o_eserarray)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::array<StructExternalSplit, 100> o_esplarray;
    for(auto & elem : o_esplarray)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    oar & o_podarray;
    oar & o_iserarray;
    oar & o_isplarray;
    oar & o_eserarray;
    oar & o_esplarray;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::array<int, 100> i_podarray;
    std::array<StructInternalSerialize, 100> i_iserarray;
    std::array<StructInternalSplit, 100>     i_isplarray;
    std::array<StructExternalSerialize, 100> i_eserarray;
    std::array<StructExternalSplit, 100>     i_esplarray;

    iar & i_podarray;
    iar & i_iserarray;
    iar & i_isplarray;
    iar & i_eserarray;
    iar & i_esplarray;

    for(size_t i=0; i<i_podarray.size(); ++i)  BOOST_CHECK_EQUAL(i_podarray[i], o_podarray[i]);
    for(size_t i=0; i<i_iserarray.size(); ++i) BOOST_CHECK(i_iserarray[i] == o_iserarray[i]);
    for(size_t i=0; i<i_isplarray.size(); ++i) BOOST_CHECK(i_isplarray[i] == o_isplarray[i]);
    for(size_t i=0; i<i_eserarray.size(); ++i) BOOST_CHECK(i_eserarray[i] == o_eserarray[i]);
    for(size_t i=0; i<i_esplarray.size(); ++i) BOOST_CHECK(i_esplarray[i] == o_esplarray[i]);
  }
}
