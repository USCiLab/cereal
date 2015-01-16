#include "common.hpp"
#include <boost/test/unit_test.hpp>

TestTypeBase::TestTypeBase()
{
}

TestTypeBase::~TestTypeBase()
{
}

TestType::TestType()
{
}

TestType::TestType(std::string s)
  : str(std::move(s))
{
}

TestType::~TestType()
{
}
