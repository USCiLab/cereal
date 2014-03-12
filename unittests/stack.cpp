#include "common.hpp"

template <class IArchive, class OArchive>
void test_stack()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::stack<int> o_podstack;
    for(int j=0; j<100; ++j)
      o_podstack.push(random_value<int>(gen));

    std::stack<StructInternalSerialize> o_iserstack;
    for(int j=0; j<100; ++j)
      o_iserstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::stack<StructInternalSplit> o_isplstack;
    for(int j=0; j<100; ++j)
      o_isplstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::stack<StructExternalSerialize> o_eserstack;
    for(int j=0; j<100; ++j)
      o_eserstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::stack<StructExternalSplit> o_esplstack;
    for(int j=0; j<100; ++j)
      o_esplstack.push({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podstack);
      oar(o_iserstack);
      oar(o_isplstack);
      oar(o_eserstack);
      oar(o_esplstack);
    }

    std::stack<int> i_podstack;
    std::stack<StructInternalSerialize> i_iserstack;
    std::stack<StructInternalSplit>     i_isplstack;
    std::stack<StructExternalSerialize> i_eserstack;
    std::stack<StructExternalSplit>     i_esplstack;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podstack);
      iar(i_iserstack);
      iar(i_isplstack);
      iar(i_eserstack);
      iar(i_esplstack);
    }

    auto & i_podstack_c  = cereal::stack_detail::container(i_podstack);
    auto & i_iserstack_c = cereal::stack_detail::container(i_iserstack);
    auto & i_isplstack_c = cereal::stack_detail::container(i_isplstack);
    auto & i_eserstack_c = cereal::stack_detail::container(i_eserstack);
    auto & i_esplstack_c = cereal::stack_detail::container(i_esplstack);

    auto & o_podstack_c  = cereal::stack_detail::container(o_podstack);
    auto & o_iserstack_c = cereal::stack_detail::container(o_iserstack);
    auto & o_isplstack_c = cereal::stack_detail::container(o_isplstack);
    auto & o_eserstack_c = cereal::stack_detail::container(o_eserstack);
    auto & o_esplstack_c = cereal::stack_detail::container(o_esplstack);

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podstack_c.begin(),    i_podstack_c.end(),    o_podstack_c.begin(),  o_podstack_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserstack_c.begin(),   i_iserstack_c.end(),   o_iserstack_c.begin(), o_iserstack_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplstack_c.begin(),   i_isplstack_c.end(),   o_isplstack_c.begin(), o_isplstack_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserstack_c.begin(),   i_eserstack_c.end(),   o_eserstack_c.begin(), o_eserstack_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplstack_c.begin(),   i_esplstack_c.end(),   o_esplstack_c.begin(), o_esplstack_c.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_stack )
{
  test_stack<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_stack )
{
  test_stack<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_stack )
{
  test_stack<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_stack )
{
  test_stack<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

