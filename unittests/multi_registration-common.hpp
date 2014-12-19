#ifndef CEREAL_MULTI_REGISTRATION_COMMON_HPP_
#define CEREAL_MULTI_REGISTRATION_COMMON_HPP_

#include "common.hpp"
#include <memory>

// Note: in this test case we ensure that multiple CEREAL_REGISTER_TYPE macro calls may exist in several cpp's for the same type.

struct TestTypeBase
{
  virtual ~TestTypeBase() {}
};

using TestTypeBasePtr = std::shared_ptr<TestTypeBase>;

struct TestType: public TestTypeBase
{
  TestType() = default;

  TestType(std::string s)
      : str(std::move(s))
  {
  }

  std::string str;
};

template <typename Archive>
void save(Archive&, const TestTypeBase&)
{
}

template <typename Archive>
void load(Archive&, TestTypeBase&)
{
}

template <typename Archive>
void save(Archive& archive, const TestType& t)
{
  std::string tmpStr = t.str + ", normally saved";
  archive(tmpStr);
}

template <typename Archive>
void load(Archive& archive, TestType& t)
{
  archive(t.str);
  t.str += ", normally loaded";
}

CEREAL_DECLARE_DYNAMIC_INIT_ENFORCER(Helper1)
CEREAL_DECLARE_DYNAMIC_INIT_ENFORCER(Helper2)

#endif // CEREAL_MULTI_REGISTRATION_COMMON_HPP_
