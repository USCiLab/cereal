#include "test_class.hpp"
#include <boost/test/unit_test.hpp>

CEREAL_DEFINE_DYNAMIC_INIT_ENFORCER(Helper)

CEREAL_REGISTER_TYPE(TestType)
