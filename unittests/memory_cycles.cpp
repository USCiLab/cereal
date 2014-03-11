#include "common.hpp"

struct MemoryCycle
{
  MemoryCycle() = default;

  MemoryCycle( int v ) :
    value( v )
  { }

  int value;
  std::weak_ptr<MemoryCycle> ptr;

  bool operator==( MemoryCycle const & other ) const
  {
    return value == other.value && ptr.lock() == other.ptr.lock();
  }

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( value, ptr );
  }
};

std::ostream& operator<<(std::ostream& os, MemoryCycle const & s)
{
  os << "[value: " << s.value << " ptr: " << s.ptr.lock() << "]";
  return os;
}

class MemoryCycleLoadAndConstruct
{
  public:
    MemoryCycleLoadAndConstruct( int v ) :
      value( v )
    { }

    MemoryCycleLoadAndConstruct( int v,
        std::weak_ptr<MemoryCycleLoadAndConstruct> p ) :
      value( v ),
      ptr( p )
    { }

    bool operator==( MemoryCycleLoadAndConstruct const & other ) const
    {
      return value == other.value && ptr.lock() == other.ptr.lock();
    }

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( value, ptr );
    }

    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<MemoryCycleLoadAndConstruct> & construct )
    {
      int value;
      std::weak_ptr<MemoryCycleLoadAndConstruct> ptr;

      ar( value, ptr );
      construct( value, ptr );
    }

    int value;
    std::weak_ptr<MemoryCycleLoadAndConstruct> ptr;
};

std::ostream& operator<<(std::ostream& os, MemoryCycleLoadAndConstruct const & s)
{
  os << "[value: " << s.value << " ptr: " << s.ptr.lock() << "]";
  return os;
}

template <class IArchive, class OArchive>
void test_memory_cycles()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    auto o_ptr1 = std::make_shared<MemoryCycle>( random_value<int>(gen) );
    o_ptr1->ptr = o_ptr1;
    auto o_ptr2 = std::make_shared<MemoryCycleLoadAndConstruct>( random_value<int>(gen) );
    o_ptr2->ptr = o_ptr2;

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_ptr1 );
      oar( o_ptr2 );
    }

    decltype(o_ptr1) i_ptr1;
    decltype(o_ptr2) i_ptr2;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_ptr1 );
      iar( i_ptr2 );
    }

    BOOST_CHECK_EQUAL( o_ptr1->value, i_ptr1->value );
    BOOST_CHECK_EQUAL( i_ptr1.get(), i_ptr1->ptr.lock().get() );
    BOOST_CHECK_EQUAL( o_ptr2->value, i_ptr2->value );
    BOOST_CHECK_EQUAL( i_ptr2.get(), i_ptr2->ptr.lock().get() );
  }
}

BOOST_AUTO_TEST_CASE( binary_memory_cycles )
{
  test_memory_cycles<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_memory_cycles )
{
  test_memory_cycles<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_memory_cycles )
{
  test_memory_cycles<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_memory_cycles )
{
  test_memory_cycles<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

