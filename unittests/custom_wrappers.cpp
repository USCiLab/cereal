#include "common.hpp"
#include <boost/test/unit_test.hpp>

namespace test_ns
{

struct B1
{
  B1() = default;
  explicit B1(bool bb):
    b(bb)
  {
  }

  operator bool() const
  {
    return b;
  }

  bool b = false;
};

template <class Archive>
void save(Archive& ar, const B1& b)
{
  ar(b.b);
}

template <class Archive>
void load(Archive& ar, B1& b)
{
  ar(b.b);
}

struct B2
{
  B2() = default;
  B2(bool bb):
    b(bb)
  {
  }

  operator bool() const
  {
    return b;
  }

  bool b = false;
};

} // namespace test_ns

namespace cereal
{

template <class Archive>
void save(Archive& ar, const test_ns::B2& b)
{
  ar(b.b);
}

template <class Archive>
void load(Archive& ar, test_ns::B2& b)
{
  ar(b.b);
}

} // namespace cereal

namespace test_ns
{

class Base
{
public:
  virtual ~Base()
  {
  }
};

template <class Archive>
void load(Archive&, Base&)
{
}

template <class Archive>
void save(Archive&, const Base&)
{
}

class Descendant: public Base
{
public:
  Descendant() = default;
  explicit Descendant(int ii, bool bb1, bool bb2):
    i(ii),
    b1(bb1),
    b2(bb2)
  {
  }

  int i = 0;
  B1 b1;
  B2 b2;
  std::vector<std::shared_ptr<Base>> v;
};

template <class Archive>
void load(Archive& ar, Descendant& d)
{
  ar(_CEREAL_NVP("i", d.i));
  ar(_CEREAL_NVP("b1", d.b1));
  ar(_CEREAL_NVP("b2", d.b2));
  ar(_CEREAL_NVP("v", d.v));
}

template <class Archive>
void save(Archive& ar, const Descendant& d)
{
  ar(_CEREAL_NVP("i", d.i));
  ar(_CEREAL_NVP("b1", d.b1));
  ar(_CEREAL_NVP("b2", d.b2));
  ar(_CEREAL_NVP("v", d.v));
}

template <template <class> class ArchiveBaseT, class Derived>
class ArchiveT: public ArchiveBaseT<Derived>
{
public:

  using Base = ArchiveBaseT<Derived>;

  template <class Stream>
  ArchiveT(Derived * derived, Stream & stream):
    ArchiveBaseT<Derived>(derived, stream)
  {
  }

  template <class T>
  void prologue(const T& t)
  {
    Base::prologue(t);

    if (std::is_same<T, Descendant>::value)
    {
      ++descendantSeenInPrologue;
    }
  }

  template <class T>
  void epilogue(const T& t)
  {
    Base::epilogue(t);

    if (std::is_same<T, Descendant>::value)
    {
      ++descendantSeenInEpilogue;
    }
  }

  template <class T>
  void processImpl(T& t)
  {
    Base::processImpl(t);

    if (std::is_same<typename std::remove_const<T>::type, Descendant>::value)
    {
      ++descendantSeenInProcess;
    }
  }

  int descendantSeenInPrologue = 0;
  int descendantSeenInEpilogue = 0;
  int descendantSeenInProcess = 0;
};

using XMLInputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::XMLInputArchiveT>::Type>;
using XMLOutputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::XMLOutputArchiveT>::Type>;
using BinaryInputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::BinaryInputArchiveT>::Type>;
using BinaryOutputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::BinaryOutputArchiveT>::Type>;
using PortableBinaryInputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::PortableBinaryInputArchiveT>::Type>;
using PortableBinaryOutputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::PortableBinaryOutputArchiveT>::Type>;
using JSONInputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::JSONInputArchiveT>::Type>;
using JSONOutputArchive = cereal::ConcreteArchive<ApplyArchiveWrapper<ArchiveT, cereal::JSONOutputArchiveT>::Type>;

template <class IArchive, class OArchive>
void test()
{
  std::stringstream stream;
  auto testVar = std::make_shared<Descendant>();
  testVar->v.push_back(std::make_shared<Descendant>(1, true, false));
  testVar->v.push_back(std::make_shared<Descendant>(2, false, true));

  {
    OArchive out(stream);
    out(std::shared_ptr<Base>(testVar));

    BOOST_CHECK_EQUAL(3, out.descendantSeenInPrologue);
    BOOST_CHECK_EQUAL(3, out.descendantSeenInEpilogue);
    BOOST_CHECK_EQUAL(3, out.descendantSeenInProcess);
  }

  std::cout << "stream: " << stream.str() << std::endl;

  stream.seekg(0);

  std::shared_ptr<Base> testVar2;

  {
    IArchive in(stream);
    in(testVar2);

    BOOST_CHECK_EQUAL(3, in.descendantSeenInPrologue);
    BOOST_CHECK_EQUAL(3, in.descendantSeenInEpilogue);
    BOOST_CHECK_EQUAL(3, in.descendantSeenInProcess);
  }

  auto descendant = dynamic_cast<Descendant*>(testVar2.get());
  BOOST_CHECK(descendant);
  BOOST_CHECK_EQUAL(0, descendant->i);
  BOOST_CHECK(!descendant->b1);
  BOOST_CHECK(!descendant->b2);
  BOOST_CHECK_EQUAL(2, descendant->v.size());

  auto innerDescendant = dynamic_cast<Descendant*>(descendant->v[0].get());
  BOOST_CHECK(innerDescendant);
  BOOST_CHECK_EQUAL(1, innerDescendant->i);
  BOOST_CHECK(innerDescendant->b1);
  BOOST_CHECK(!innerDescendant->b2);

  innerDescendant = dynamic_cast<Descendant*>(descendant->v[1].get());
  BOOST_CHECK(innerDescendant);
  BOOST_CHECK_EQUAL(2, innerDescendant->i);
  BOOST_CHECK(!innerDescendant->b1);
  BOOST_CHECK(innerDescendant->b2);
}

} // namespace test_ns

BOOST_AUTO_TEST_CASE(xml)
{
  test_ns::test<test_ns::XMLInputArchive, test_ns::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE(binary)
{
  test_ns::test<test_ns::BinaryInputArchive, test_ns::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE(portable_binary)
{
  test_ns::test<test_ns::PortableBinaryInputArchive, test_ns::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE(json)
{
  test_ns::test<test_ns::JSONInputArchive, test_ns::JSONOutputArchive>();
}

CEREAL_REGISTER_ARCHIVE(test_ns::XMLInputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::XMLOutputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::BinaryInputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::BinaryOutputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::PortableBinaryInputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::PortableBinaryOutputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::JSONInputArchive)
CEREAL_REGISTER_ARCHIVE(test_ns::JSONOutputArchive)
CEREAL_REGISTER_TYPE(test_ns::Descendant)
