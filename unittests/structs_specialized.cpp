#include "common.hpp"

class SpecializedMSerialize
{
  public:
    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( x );
    }
};

class SpecializedMSplit
{
  public:
    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    void save( Archive & ar ) const
    {
      ar( x );
    }

    template <class Archive>
    void load( Archive & ar )
    {
      ar( x );
    }
};

class SpecializedNMSerialize
{
  public:
    int x;
};

template <class Archive>
void serialize( Archive & ar, SpecializedNMSerialize & s )
{
  ar( s.x );
}

class SpecializedNMSplit
{
  public:
    int x;
};

template <class Archive>
void load( Archive & ar, SpecializedNMSplit & s )
{
  ar( s.x );
}

template <class Archive>
void save( Archive & ar, SpecializedNMSplit const & s )
{
  ar( s.x );
}

namespace cereal
{
  template <class Archive> struct specialize<Archive, SpecializedMSerialize, cereal::specialization::member_serialize> {};
  template <class Archive> struct specialize<Archive, SpecializedMSplit, cereal::specialization::member_load_save> {};
  template <class Archive> struct specialize<Archive, SpecializedNMSerialize, cereal::specialization::non_member_serialize> {};
  template <class Archive> struct specialize<Archive, SpecializedNMSplit, cereal::specialization::non_member_load_save> {};
}

template <class IArchive, class OArchive>
void test_structs_specialized()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    SpecializedMSerialize  o_iser = { random_value<int>(gen) };
    SpecializedMSplit      o_ispl = { random_value<int>(gen) };
    SpecializedNMSerialize o_eser = { random_value<int>(gen) };
    SpecializedNMSplit     o_espl = { random_value<int>(gen) };

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_iser, o_ispl, o_eser, o_espl);
    }

    decltype(o_iser) i_iser;
    decltype(o_ispl) i_ispl;
    decltype(o_eser) i_eser;
    decltype(o_espl) i_espl;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_iser, i_ispl, i_eser, i_espl);
    }

    BOOST_CHECK(i_iser.x == o_iser.x);
    BOOST_CHECK(i_ispl.x == o_ispl.x);
    BOOST_CHECK(i_eser.x == o_eser.x);
    BOOST_CHECK(i_espl.x == o_espl.x);
  }
}

BOOST_AUTO_TEST_CASE( binary_structs_specialized )
{
  test_structs_specialized<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( portable_binary_structs_specialized )
{
  test_structs_specialized<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE( xml_structs_specialized )
{
  test_structs_specialized<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

BOOST_AUTO_TEST_CASE( json_structs_specialized )
{
  test_structs_specialized<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

