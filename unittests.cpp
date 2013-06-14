#include <cereal/binary_archive/binary_archive.hpp>
#include <cereal/binary_archive/memory.hpp>
#include <cereal/binary_archive/array.hpp>
#include <cereal/binary_archive/vector.hpp>
#include <cereal/binary_archive/deque.hpp>
#include <cereal/binary_archive/forward_list.hpp>
#include <cereal/binary_archive/list.hpp>
#include <cereal/binary_archive/string.hpp>
#include <cereal/binary_archive/map.hpp>
#include <cereal/binary_archive/queue.hpp>
#include <cereal/binary_archive/set.hpp>
#include <limits>
#include <random>

namespace boost
{
  template<class F, class S>
  ::std::ostream & operator<<(::std::ostream & os, ::std::pair<F, S> const & p)
  {
    os << "([" << p.first << "], [" << p.second << "])";
    return os;
  }
}

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Cereal
#include <boost/test/unit_test.hpp>

struct StructBase
{
  int x, y;
  bool operator==(StructBase const & other) const
  { return x == other.x && y == other.y; }
  bool operator!=(StructBase const & other) const
  { return x != other.x || y != other.y; }
  bool operator<(StructBase const & other) const
  { 
    if (x < other.x) return true;
    else if(other.x < x) return false;
    else return (y < other.y);
  }
};

std::ostream& operator<<(std::ostream& os, StructBase const & s)
{
    os << "[x: " << s.x << " y: " << s.y << "]";
    return os;
}


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
    void save(Archive & ar) const
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
void save(Archive & ar, StructExternalSplit const & s)
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

template<class T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
random_value(std::mt19937 & gen)
{
  std::string s(std::uniform_int_distribution<int>(3, 30)(gen), ' '); 
  for(char & c : s)
    c = std::uniform_int_distribution<char>(' ', '~')(gen); 
  return s;
}

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

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podarray.begin(),    i_podarray.end(),    o_podarray.begin(),  o_podarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserarray.begin(),   i_iserarray.end(),   o_iserarray.begin(), o_iserarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplarray.begin(),   i_isplarray.end(),   o_isplarray.begin(), o_isplarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserarray.begin(),   i_eserarray.end(),   o_eserarray.begin(), o_eserarray.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplarray.begin(),   i_esplarray.end(),   o_esplarray.begin(), o_esplarray.end());
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_vector )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::vector<int> o_podvector(100);
    for(auto & elem : o_podvector)
      elem = random_value<decltype(o_podvector)::value_type>(gen);

    std::vector<StructInternalSerialize> o_iservector(100);
    for(auto & elem : o_iservector)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::vector<StructInternalSplit> o_isplvector(100);
    for(auto & elem : o_isplvector)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::vector<StructExternalSerialize> o_eservector(100);
    for(auto & elem : o_eservector)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::vector<StructExternalSplit> o_esplvector(100);
    for(auto & elem : o_esplvector)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    oar & o_podvector;
    oar & o_iservector;
    oar & o_isplvector;
    oar & o_eservector;
    oar & o_esplvector;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::vector<int> i_podvector;
    std::vector<StructInternalSerialize> i_iservector;
    std::vector<StructInternalSplit>     i_isplvector;
    std::vector<StructExternalSerialize> i_eservector;
    std::vector<StructExternalSplit>     i_esplvector;

    iar & i_podvector;
    iar & i_iservector;
    iar & i_isplvector;
    iar & i_eservector;
    iar & i_esplvector;

    BOOST_CHECK_EQUAL(i_podvector.size(),  o_podvector.size());
    BOOST_CHECK_EQUAL(i_iservector.size(), o_iservector.size());
    BOOST_CHECK_EQUAL(i_isplvector.size(), o_isplvector.size());
    BOOST_CHECK_EQUAL(i_eservector.size(), o_eservector.size());
    BOOST_CHECK_EQUAL(i_esplvector.size(), o_esplvector.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podvector.begin(),    i_podvector.end(),    o_podvector.begin(),  o_podvector.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iservector.begin(),   i_iservector.end(),   o_iservector.begin(), o_iservector.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplvector.begin(),   i_isplvector.end(),   o_isplvector.begin(), o_isplvector.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eservector.begin(),   i_eservector.end(),   o_eservector.begin(), o_eservector.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplvector.begin(),   i_esplvector.end(),   o_esplvector.begin(), o_esplvector.end());
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_deque )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::deque<int> o_poddeque(100);
    for(auto & elem : o_poddeque)
      elem = random_value<decltype(o_poddeque)::value_type>(gen);

    std::deque<StructInternalSerialize> o_iserdeque(100);
    for(auto & elem : o_iserdeque)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::deque<StructInternalSplit> o_ispldeque(100);
    for(auto & elem : o_ispldeque)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::deque<StructExternalSerialize> o_eserdeque(100);
    for(auto & elem : o_eserdeque)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::deque<StructExternalSplit> o_espldeque(100);
    for(auto & elem : o_espldeque)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    oar & o_poddeque;
    oar & o_iserdeque;
    oar & o_ispldeque;
    oar & o_eserdeque;
    oar & o_espldeque;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::deque<int> i_poddeque;
    std::deque<StructInternalSerialize> i_iserdeque;
    std::deque<StructInternalSplit>     i_ispldeque;
    std::deque<StructExternalSerialize> i_eserdeque;
    std::deque<StructExternalSplit>     i_espldeque;

    iar & i_poddeque;
    iar & i_iserdeque;
    iar & i_ispldeque;
    iar & i_eserdeque;
    iar & i_espldeque;

    BOOST_CHECK_EQUAL(i_poddeque.size(),  o_poddeque.size());
    BOOST_CHECK_EQUAL(i_iserdeque.size(), o_iserdeque.size());
    BOOST_CHECK_EQUAL(i_ispldeque.size(), o_ispldeque.size());
    BOOST_CHECK_EQUAL(i_eserdeque.size(), o_eserdeque.size());
    BOOST_CHECK_EQUAL(i_espldeque.size(), o_espldeque.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(i_poddeque.begin(),    i_poddeque.end(),    o_poddeque.begin(),  o_poddeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserdeque.begin(),   i_iserdeque.end(),   o_iserdeque.begin(), o_iserdeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_ispldeque.begin(),   i_ispldeque.end(),   o_ispldeque.begin(), o_ispldeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserdeque.begin(),   i_eserdeque.end(),   o_eserdeque.begin(), o_eserdeque.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_espldeque.begin(),   i_espldeque.end(),   o_espldeque.begin(), o_espldeque.end());
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_forward_list )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::forward_list<int> o_podforward_list(100);
    for(auto & elem : o_podforward_list)
      elem = random_value<decltype(o_podforward_list)::value_type>(gen);

    std::forward_list<StructInternalSerialize> o_iserforward_list(100);
    for(auto & elem : o_iserforward_list)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::forward_list<StructInternalSplit> o_isplforward_list(100);
    for(auto & elem : o_isplforward_list)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::forward_list<StructExternalSerialize> o_eserforward_list(100);
    for(auto & elem : o_eserforward_list)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::forward_list<StructExternalSplit> o_esplforward_list(100);
    for(auto & elem : o_esplforward_list)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    oar & o_podforward_list;
    oar & o_iserforward_list;
    oar & o_isplforward_list;
    oar & o_eserforward_list;
    oar & o_esplforward_list;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::forward_list<int> i_podforward_list;
    std::forward_list<StructInternalSerialize> i_iserforward_list;
    std::forward_list<StructInternalSplit>     i_isplforward_list;
    std::forward_list<StructExternalSerialize> i_eserforward_list;
    std::forward_list<StructExternalSplit>     i_esplforward_list;

    iar & i_podforward_list;
    iar & i_iserforward_list;
    iar & i_isplforward_list;
    iar & i_eserforward_list;
    iar & i_esplforward_list;

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podforward_list.begin(),    i_podforward_list.end(),    o_podforward_list.begin(),  o_podforward_list.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserforward_list.begin(),   i_iserforward_list.end(),   o_iserforward_list.begin(), o_iserforward_list.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplforward_list.begin(),   i_isplforward_list.end(),   o_isplforward_list.begin(), o_isplforward_list.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserforward_list.begin(),   i_eserforward_list.end(),   o_eserforward_list.begin(), o_eserforward_list.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplforward_list.begin(),   i_esplforward_list.end(),   o_esplforward_list.begin(), o_esplforward_list.end());
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_list )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::list<int> o_podlist(100);
    for(auto & elem : o_podlist)
      elem = random_value<decltype(o_podlist)::value_type>(gen);

    std::list<StructInternalSerialize> o_iserlist(100);
    for(auto & elem : o_iserlist)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::list<StructInternalSplit> o_ispllist(100);
    for(auto & elem : o_ispllist)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::list<StructExternalSerialize> o_eserlist(100);
    for(auto & elem : o_eserlist)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    std::list<StructExternalSplit> o_espllist(100);
    for(auto & elem : o_espllist)
      elem = { random_value<int>(gen), random_value<int>(gen) };

    oar & o_podlist;
    oar & o_iserlist;
    oar & o_ispllist;
    oar & o_eserlist;
    oar & o_espllist;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::list<int> i_podlist;
    std::list<StructInternalSerialize> i_iserlist;
    std::list<StructInternalSplit>     i_ispllist;
    std::list<StructExternalSerialize> i_eserlist;
    std::list<StructExternalSplit>     i_espllist;

    iar & i_podlist;
    iar & i_iserlist;
    iar & i_ispllist;
    iar & i_eserlist;
    iar & i_espllist;

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podlist.begin(),    i_podlist.end(),    o_podlist.begin(),  o_podlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserlist.begin(),   i_iserlist.end(),   o_iserlist.begin(), o_iserlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_ispllist.begin(),   i_ispllist.end(),   o_ispllist.begin(), o_ispllist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserlist.begin(),   i_eserlist.end(),   o_eserlist.begin(), o_eserlist.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_espllist.begin(),   i_espllist.end(),   o_espllist.begin(), o_espllist.end());
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_map )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::map<std::string, int> o_podmap;
    for(int j=0; j<100; ++j)
      o_podmap.insert({random_value<std::string>(gen), random_value<int>(gen)});

    std::map<double, StructInternalSerialize> o_isermap;
    for(int j=0; j<100; ++j)
      o_isermap.insert({random_value<double>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::map<float, StructInternalSplit> o_isplmap;
    for(int j=0; j<100; ++j)
      o_isplmap.insert({random_value<float>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::map<uint32_t, StructExternalSerialize> o_esermap;
    for(int j=0; j<100; ++j)
      o_esermap.insert({random_value<uint32_t>(gen), { random_value<int>(gen), random_value<int>(gen) }});

    std::map<int8_t, StructExternalSplit> o_esplmap;
    for(int j=0; j<100; ++j)
      o_esplmap.insert({random_value<char>(gen),  { random_value<int>(gen), random_value<int>(gen) }});

    oar & o_podmap;
    oar & o_isermap;
    oar & o_isplmap;
    oar & o_esermap;
    oar & o_esplmap;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::map<std::string, int> i_podmap;
    std::map<double, StructInternalSerialize>   i_isermap;
    std::map<float, StructInternalSplit>        i_isplmap;
    std::map<uint32_t, StructExternalSerialize> i_esermap;
    std::map<int8_t, StructExternalSplit>       i_esplmap;

    iar & i_podmap;
    iar & i_isermap;
    iar & i_isplmap;
    iar & i_esermap;
    iar & i_esplmap;

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podmap.begin(),    i_podmap.end(),    o_podmap.begin(),  o_podmap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isermap.begin(),   i_isermap.end(),   o_isermap.begin(), o_isermap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplmap.begin(),   i_isplmap.end(),   o_isplmap.begin(), o_isplmap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esermap.begin(),   i_esermap.end(),   o_esermap.begin(), o_esermap.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplmap.begin(),   i_esplmap.end(),   o_esplmap.begin(), o_esplmap.end());
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
    BOOST_CHECK_EQUAL(*i_xptr1,      *i_xptr2);

    BOOST_CHECK_EQUAL(o_yptr1.get(), o_yptr2.get());
    BOOST_CHECK_EQUAL(i_yptr1.get(), i_yptr2.get());
    BOOST_CHECK_EQUAL(*i_yptr1,      *i_yptr2);
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_queue )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::queue<int> o_podqueue;
    for(int j=0; j<100; ++j)
      o_podqueue.push(random_value<int>(gen));

    std::queue<StructInternalSerialize> o_iserqueue;
    for(int j=0; j<100; ++j)
      o_iserqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::queue<StructInternalSplit> o_isplqueue;
    for(int j=0; j<100; ++j)
      o_isplqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::queue<StructExternalSerialize> o_eserqueue;
    for(int j=0; j<100; ++j)
      o_eserqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::queue<StructExternalSplit> o_esplqueue;
    for(int j=0; j<100; ++j)
      o_esplqueue.push({ random_value<int>(gen), random_value<int>(gen) });

    oar & o_podqueue;
    oar & o_iserqueue;
    oar & o_isplqueue;
    oar & o_eserqueue;
    oar & o_esplqueue;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::queue<int> i_podqueue;
    std::queue<StructInternalSerialize> i_iserqueue;
    std::queue<StructInternalSplit>     i_isplqueue;
    std::queue<StructExternalSerialize> i_eserqueue;
    std::queue<StructExternalSplit>     i_esplqueue;

    iar & i_podqueue;
    iar & i_iserqueue;
    iar & i_isplqueue;
    iar & i_eserqueue;
    iar & i_esplqueue;

    auto & i_podqueue_c  = cereal::queue_detail::container(i_podqueue);
    auto & i_iserqueue_c = cereal::queue_detail::container(i_iserqueue);
    auto & i_isplqueue_c = cereal::queue_detail::container(i_isplqueue);
    auto & i_eserqueue_c = cereal::queue_detail::container(i_eserqueue);
    auto & i_esplqueue_c = cereal::queue_detail::container(i_esplqueue);

    auto & o_podqueue_c  = cereal::queue_detail::container(o_podqueue);
    auto & o_iserqueue_c = cereal::queue_detail::container(o_iserqueue);
    auto & o_isplqueue_c = cereal::queue_detail::container(o_isplqueue);
    auto & o_eserqueue_c = cereal::queue_detail::container(o_eserqueue);
    auto & o_esplqueue_c = cereal::queue_detail::container(o_esplqueue);

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podqueue_c.begin(),    i_podqueue_c.end(),    o_podqueue_c.begin(),  o_podqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserqueue_c.begin(),   i_iserqueue_c.end(),   o_iserqueue_c.begin(), o_iserqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplqueue_c.begin(),   i_isplqueue_c.end(),   o_isplqueue_c.begin(), o_isplqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserqueue_c.begin(),   i_eserqueue_c.end(),   o_eserqueue_c.begin(), o_eserqueue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplqueue_c.begin(),   i_esplqueue_c.end(),   o_esplqueue_c.begin(), o_esplqueue_c.end());
  }
}

// ######################################################################
BOOST_AUTO_TEST_CASE( binary_set )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int i=0; i<100; ++i)
  {
    std::ostringstream os;
    cereal::BinaryOutputArchive oar(os);

    std::set<int> o_podset;
    for(int j=0; j<100; ++j)
      o_podset.insert(random_value<int>(gen));

    std::set<StructInternalSerialize> o_iserset;
    for(int j=0; j<100; ++j)
      o_iserset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructInternalSplit> o_isplset;
    for(int j=0; j<100; ++j)
      o_isplset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructExternalSerialize> o_eserset;
    for(int j=0; j<100; ++j)
      o_eserset.insert({ random_value<int>(gen), random_value<int>(gen) });

    std::set<StructExternalSplit> o_esplset;
    for(int j=0; j<100; ++j)
      o_esplset.insert({ random_value<int>(gen), random_value<int>(gen) });

    oar & o_podset;
    oar & o_iserset;
    oar & o_isplset;
    oar & o_eserset;
    oar & o_esplset;

    std::istringstream is(os.str());
    cereal::BinaryInputArchive iar(is);

    std::set<int> i_podset;
    std::set<StructInternalSerialize>   i_iserset;
    std::set<StructInternalSplit>        i_isplset;
    std::set<StructExternalSerialize> i_eserset;
    std::set<StructExternalSplit>       i_esplset;

    iar & i_podset;
    iar & i_iserset;
    iar & i_isplset;
    iar & i_eserset;
    iar & i_esplset;

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podset.begin(),    i_podset.end(),    o_podset.begin(),  o_podset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserset.begin(),   i_iserset.end(),   o_iserset.begin(), o_iserset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplset.begin(),   i_isplset.end(),   o_isplset.begin(), o_isplset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserset.begin(),   i_eserset.end(),   o_eserset.begin(), o_eserset.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplset.begin(),   i_esplset.end(),   o_esplset.begin(), o_esplset.end());
  }
}
