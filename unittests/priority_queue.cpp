#include "common.hpp"

template <class IArchive, class OArchive>
void test_priority_queue()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    std::priority_queue<int> o_podpriority_queue;
    for(int j=0; j<100; ++j)
      o_podpriority_queue.push(random_value<int>(gen));

    std::priority_queue<StructInternalSerialize> o_iserpriority_queue;
    for(int j=0; j<100; ++j)
      o_iserpriority_queue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::priority_queue<StructInternalSplit> o_isplpriority_queue;
    for(int j=0; j<100; ++j)
      o_isplpriority_queue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::priority_queue<StructExternalSerialize> o_eserpriority_queue;
    for(int j=0; j<100; ++j)
      o_eserpriority_queue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::priority_queue<StructExternalSplit> o_esplpriority_queue;
    for(int j=0; j<100; ++j)
      o_esplpriority_queue.push({ random_value<int>(gen), random_value<int>(gen) });

    std::ostringstream os;
    {
      OArchive oar(os);

      oar(o_podpriority_queue);
      oar(o_iserpriority_queue);
      oar(o_isplpriority_queue);
      oar(o_eserpriority_queue);
      oar(o_esplpriority_queue);
    }

    std::priority_queue<int> i_podpriority_queue;
    std::priority_queue<StructInternalSerialize> i_iserpriority_queue;
    std::priority_queue<StructInternalSplit>     i_isplpriority_queue;
    std::priority_queue<StructExternalSerialize> i_eserpriority_queue;
    std::priority_queue<StructExternalSplit>     i_esplpriority_queue;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar(i_podpriority_queue);
      iar(i_iserpriority_queue);
      iar(i_isplpriority_queue);
      iar(i_eserpriority_queue);
      iar(i_esplpriority_queue);
    }

    auto & i_podpriority_queue_c  = cereal::queue_detail::container(i_podpriority_queue);
    auto & i_iserpriority_queue_c = cereal::queue_detail::container(i_iserpriority_queue);
    auto & i_isplpriority_queue_c = cereal::queue_detail::container(i_isplpriority_queue);
    auto & i_eserpriority_queue_c = cereal::queue_detail::container(i_eserpriority_queue);
    auto & i_esplpriority_queue_c = cereal::queue_detail::container(i_esplpriority_queue);

    auto & o_podpriority_queue_c  = cereal::queue_detail::container(o_podpriority_queue);
    auto & o_iserpriority_queue_c = cereal::queue_detail::container(o_iserpriority_queue);
    auto & o_isplpriority_queue_c = cereal::queue_detail::container(o_isplpriority_queue);
    auto & o_eserpriority_queue_c = cereal::queue_detail::container(o_eserpriority_queue);
    auto & o_esplpriority_queue_c = cereal::queue_detail::container(o_esplpriority_queue);

    BOOST_CHECK_EQUAL_COLLECTIONS(i_podpriority_queue_c.begin(),    i_podpriority_queue_c.end(),    o_podpriority_queue_c.begin(),  o_podpriority_queue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_iserpriority_queue_c.begin(),   i_iserpriority_queue_c.end(),   o_iserpriority_queue_c.begin(), o_iserpriority_queue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_isplpriority_queue_c.begin(),   i_isplpriority_queue_c.end(),   o_isplpriority_queue_c.begin(), o_isplpriority_queue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_eserpriority_queue_c.begin(),   i_eserpriority_queue_c.end(),   o_eserpriority_queue_c.begin(), o_eserpriority_queue_c.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(i_esplpriority_queue_c.begin(),   i_esplpriority_queue_c.end(),   o_esplpriority_queue_c.begin(), o_esplpriority_queue_c.end());
  }
}

BOOST_AUTO_TEST_CASE( binary_priority_queue )
{
  test_priority_queue<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_priority_queue )
{
  test_priority_queue<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_priority_queue )
{
  test_priority_queue<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_priority_queue )
{
  test_priority_queue<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

