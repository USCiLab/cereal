#ifndef CEREAL_DESERIALIZE_ONLY_TEST_CLASS_HPP_
#define CEREAL_DESERIALIZE_ONLY_TEST_CLASS_HPP_

#include "../common.hpp"
#include <memory>

struct TestTypeBase
{
  TestTypeBase();
  virtual ~TestTypeBase();
  
  template <typename Archive>
  void serialize(Archive&)
  {
  }
};

using TestTypeBasePtr = std::shared_ptr<TestTypeBase>;

struct TestType: public TestTypeBase
{
  TestType();
  TestType(std::string s);
  ~TestType();

  template <typename Archive>
  void serialize(Archive& ar)
  {
    ar(str);
  }

  std::string str;
};

CEREAL_DECLARE_DYNAMIC_INIT_ENFORCER(Helper)

#endif // CEREAL_DESERIALIZE_ONLY_TEST_CLASS_HPP_
