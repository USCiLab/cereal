#include "common.hpp"

class VersionStructMS
{
  public:
    bool x;
    std::uint32_t v;

  private:
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar, std::uint32_t const version )
    {
      ar( x );
      v = version;
    }
};

struct VersionStructMSP
{
  uint8_t x;
  std::uint32_t v;
  template <class Archive>
  void save( Archive & ar, std::uint32_t const /*version*/ ) const
  {
    ar( x );
  }

  template <class Archive>
  void load( Archive & ar, std::uint32_t const version )
  {
    ar( x );
    v = version;
  }
};

struct VersionStructNMS
{
  std::int32_t x;
  std::uint32_t v;
};

template <class Archive>
void serialize( Archive & ar, VersionStructNMS & vnms, const std::uint32_t version )
{
  ar( vnms.x );
  vnms.v = version;
}

struct VersionStructNMSP
{
  double x;
  std::uint32_t v;
};

template <class Archive>
void save( Archive & ar, VersionStructNMSP const & vnms, const std::uint32_t /*version*/ )
{
  ar( vnms.x );
}

template <class Archive>
void load( Archive & ar, VersionStructNMSP & vnms, const std::uint32_t version )
{
  ar( vnms.x );
  vnms.v = version;
}

CEREAL_CLASS_VERSION( VersionStructMSP, 33 )
CEREAL_CLASS_VERSION( VersionStructNMS, 66 )
CEREAL_CLASS_VERSION( VersionStructNMSP, 99 )

template <class IArchive, class OArchive>
void test_versioning()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    VersionStructMS o_MS      = {random_value<uint8_t>(gen) % 2 ? true : false, 1};
    VersionStructMSP o_MSP    = {random_value<uint8_t>(gen), 1};
    VersionStructNMS o_NMS    = {random_value<int32_t>(gen), 1};
    VersionStructNMSP o_NMSP  = {random_value<double>(gen), 1};
    VersionStructMS o_MS2     = {random_value<uint8_t>(gen) % 2 ? true : false, 1};
    VersionStructMSP o_MSP2   = {random_value<uint8_t>(gen), 1};
    VersionStructNMS o_NMS2   = {random_value<int32_t>(gen), 1};
    VersionStructNMSP o_NMSP2 = {random_value<double>(gen), 1};

    std::ostringstream os;
    {
      OArchive oar(os);
      oar( o_MS );
      oar( o_MSP );
      oar( o_NMS );
      oar( o_NMSP );
      oar( o_MS2 );
      oar( o_MSP2 );
      oar( o_NMS2 );
      oar( o_NMSP2 );
    }

    decltype(o_MS) i_MS;
    decltype(o_MSP) i_MSP;
    decltype(o_NMS) i_NMS;
    decltype(o_NMSP) i_NMSP;
    decltype(o_MS2) i_MS2;
    decltype(o_MSP2) i_MSP2;
    decltype(o_NMS2) i_NMS2;
    decltype(o_NMSP2) i_NMSP2;

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      iar( i_MS );
      iar( i_MSP );
      iar( i_NMS );
      iar( i_NMSP );
      iar( i_MS2 );
      iar( i_MSP2 );
      iar( i_NMS2 );
      iar( i_NMSP2 );
    }

    BOOST_CHECK_EQUAL(o_MS.x, i_MS.x);
    BOOST_CHECK_EQUAL(i_MS.v, 0);
    BOOST_CHECK_EQUAL(o_MSP.x, i_MSP.x);
    BOOST_CHECK_EQUAL(i_MSP.v, 33);
    BOOST_CHECK_EQUAL(o_NMS.x, i_NMS.x);
    BOOST_CHECK_EQUAL(i_NMS.v, 66);
    BOOST_CHECK_CLOSE(o_NMSP.x, i_NMSP.x, 1e-5);
    BOOST_CHECK_EQUAL(i_NMSP.v, 99);

    BOOST_CHECK_EQUAL(o_MS2.x, i_MS2.x);
    BOOST_CHECK_EQUAL(i_MS2.v, 0);
    BOOST_CHECK_EQUAL(o_MSP2.x, i_MSP2.x);
    BOOST_CHECK_EQUAL(i_MSP2.v, 33);
    BOOST_CHECK_EQUAL(o_NMS2.x, i_NMS2.x);
    BOOST_CHECK_EQUAL(i_NMS2.v, 66);
    BOOST_CHECK_CLOSE(o_NMSP2.x, i_NMSP2.x, 1e-5);
    BOOST_CHECK_EQUAL(i_NMSP2.v, 99);
    }
}

BOOST_AUTO_TEST_CASE( binary_versioning )
{
  test_versioning<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_versioning )
{
  test_versioning<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_versioning )
{
  test_versioning<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_versioning )
{
  test_versioning<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

