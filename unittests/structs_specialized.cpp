/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES AND SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "common.hpp"
#include <boost/test/unit_test.hpp>

struct BogusBase
{
  template <class Archive>
  void serialize( Archive & ) {}

  template <class Archive>
  void save( Archive & ) const {}

  template <class Archive>
  void load( Archive & ) {}

  template <class Archive>
  int save_minimal( Archive const & ) const { return 0; }

  template <class Archive>
  void load_minimal( Archive const &, int const & ) {}
};

struct BogusBaseVersioned
{
  template <class Archive>
  void serialize( Archive &, const std::uint32_t ) {}

  template <class Archive>
  void save( Archive &, const std::uint32_t ) const {}

  template <class Archive>
  void load( Archive &, const std::uint32_t ) {}

  template <class Archive>
  int save_minimal( Archive const &, const std::uint32_t ) const { return 0; }

  template <class Archive>
  void load_minimal( Archive const &, int const &, const std::uint32_t ) {}
};

class SpecializedMSerialize : public BogusBase
{
  public:
    SpecializedMSerialize() = default;
    SpecializedMSerialize( int xx ) : x(xx) {}

    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( x );
    }
};

class SpecializedMSerializeVersioned : public BogusBaseVersioned
{
  public:
    SpecializedMSerializeVersioned() = default;
    SpecializedMSerializeVersioned( int xx ) : x(xx) {}

    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar, const std::uint32_t )
    {
      ar( x );
    }
};

class SpecializedMSplit : public BogusBase
{
  public:
    SpecializedMSplit() = default;
    SpecializedMSplit( int xx ) : x(xx) {}

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

class SpecializedMSplitVersioned : public BogusBaseVersioned
{
  public:
    SpecializedMSplitVersioned() = default;
    SpecializedMSplitVersioned( int xx ) : x(xx) {}

    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    void save( Archive & ar, const std::uint32_t ) const
    {
      ar( x );
    }

    template <class Archive>
    void load( Archive & ar, const std::uint32_t )
    {
      ar( x );
    }
};

class SpecializedMSplitMinimal : public BogusBase
{
  public:
    SpecializedMSplitMinimal() = default;
    SpecializedMSplitMinimal( int xx ) : x(xx) {}

    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    int save_minimal( Archive const & ) const
    {
      return x;
    }

    template <class Archive>
    void load_minimal( Archive const &, int const & value )
    {
      x = value;
    }
};

class SpecializedMSplitVersionedMinimal : public BogusBaseVersioned
{
  public:
    SpecializedMSplitVersionedMinimal() = default;
    SpecializedMSplitVersionedMinimal( int xx ) : x(xx) {}

    int x;

  private:
    friend class cereal::access;
    template <class Archive>
    int save_minimal( Archive const &, const std::uint32_t ) const
    {
      return x;
    }

    template <class Archive>
    void load_minimal( Archive const &, int const & value, const std::uint32_t )
    {
      x = value;
    }
};

class SpecializedNMSerialize : public BogusBase
{
  public:
    SpecializedNMSerialize() = default;
    SpecializedNMSerialize( int xx ) : x(xx) {}

    int x;
};

template <class Archive>
void serialize( Archive & ar, SpecializedNMSerialize & s )
{
  ar( s.x );
}

class SpecializedNMSerializeVersioned : public BogusBaseVersioned
{
  public:
    SpecializedNMSerializeVersioned() = default;
    SpecializedNMSerializeVersioned( int xx ) : x(xx) {}

    int x;
};

template <class Archive>
void serialize( Archive & ar, SpecializedNMSerializeVersioned & s )
{
  ar( s.x );
}

class SpecializedNMSplit : public BogusBase
{
  public:
    SpecializedNMSplit() = default;
    SpecializedNMSplit( int xx ) : x(xx) {}

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

class SpecializedNMSplitVersioned : public BogusBaseVersioned
{
  public:
    SpecializedNMSplitVersioned() = default;
    SpecializedNMSplitVersioned( int xx ) : x(xx) {}

    int x;
};

template <class Archive>
void load( Archive & ar, SpecializedNMSplitVersioned & s, const std::uint32_t )
{
  ar( s.x );
}

template <class Archive>
void save( Archive & ar, SpecializedNMSplitVersioned const & s, const std::uint32_t )
{
  ar( s.x );
}

class SpecializedNMSplitMinimal : public BogusBase
{
  public:
    SpecializedNMSplitMinimal() = default;
    SpecializedNMSplitMinimal( int xx ) : x(xx) {}

    int x;
};

template <class Archive>
void load_minimal( Archive const &, SpecializedNMSplitMinimal & s, int const & value )
{
  s.x = value;
}

template <class Archive>
int save_minimal( Archive const &, SpecializedNMSplitMinimal const & s )
{
  return s.x;
}

class SpecializedNMSplitVersionedMinimal : public BogusBaseVersioned
{
  public:
    SpecializedNMSplitVersionedMinimal() = default;
    SpecializedNMSplitVersionedMinimal( int xx ) : x(xx) {}

    int x;
};

template <class Archive>
void load_minimal( Archive const &, SpecializedNMSplitVersionedMinimal & s, int const & value, const std::uint32_t )
{
  s.x = value;
}

template <class Archive>
int save_minimal( Archive const &, SpecializedNMSplitVersionedMinimal const & s, const std::uint32_t )
{
  return s.x;
}

namespace cereal
{
  template <class Archive> struct specialize<Archive, SpecializedMSerialize, cereal::specialization::member_serialize> {};
  template <class Archive> struct specialize<Archive, SpecializedMSerializeVersioned, cereal::specialization::member_serialize> {};

  template <class Archive> struct specialize<Archive, SpecializedMSplit, cereal::specialization::member_load_save> {};
  template <class Archive> struct specialize<Archive, SpecializedMSplitVersioned, cereal::specialization::member_load_save> {};

  template <class Archive> struct specialize<Archive, SpecializedMSplitMinimal, cereal::specialization::member_load_save_minimal> {};
  template <class Archive> struct specialize<Archive, SpecializedMSplitVersionedMinimal, cereal::specialization::member_load_save_minimal> {};

  template <class Archive> struct specialize<Archive, SpecializedNMSerialize, cereal::specialization::non_member_serialize> {};
  template <class Archive> struct specialize<Archive, SpecializedNMSerializeVersioned, cereal::specialization::non_member_serialize> {};

  template <class Archive> struct specialize<Archive, SpecializedNMSplit, cereal::specialization::non_member_load_save> {};
  template <class Archive> struct specialize<Archive, SpecializedNMSplitVersioned, cereal::specialization::non_member_load_save> {};

  template <class Archive> struct specialize<Archive, SpecializedNMSplitMinimal,
           cereal::specialization::non_member_load_save_minimal> {};
  template <class Archive> struct specialize<Archive, SpecializedNMSplitVersionedMinimal,
           cereal::specialization::non_member_load_save_minimal> {};
}

template <class IArchive, class OArchive>
void test_structs_specialized()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    SpecializedMSerialize  o_iser                   = { random_value<int>(gen) };
    SpecializedMSerializeVersioned  o_iserv         = { random_value<int>(gen) };

    SpecializedMSplit      o_ispl                   = { random_value<int>(gen) };
    SpecializedMSplitVersioned      o_isplv         = { random_value<int>(gen) };

    SpecializedMSplitMinimal      o_isplm           = { random_value<int>(gen) };
    SpecializedMSplitVersionedMinimal      o_isplvm = { random_value<int>(gen) };

    SpecializedNMSerialize o_eser                   = { random_value<int>(gen) };
    SpecializedNMSerializeVersioned o_eserv         = { random_value<int>(gen) };

    SpecializedNMSplit     o_espl                   = { random_value<int>(gen) };
    SpecializedNMSplitVersioned     o_esplv         = { random_value<int>(gen) };

    SpecializedNMSplitMinimal     o_esplm           = { random_value<int>(gen) };
    SpecializedNMSplitVersionedMinimal     o_esplvm = { random_value<int>(gen) };

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_iser, o_iserv,
           o_ispl, o_isplv,
           o_isplm, o_isplvm,
           o_eser, o_eserv,
           o_espl, o_esplv,
           o_esplm, o_esplvm );
    }

    decltype(o_iser) i_iser;
    decltype(o_iserv) i_iserv;

    decltype(o_ispl) i_ispl;
    decltype(o_isplv) i_isplv;

    decltype(o_isplm) i_isplm;
    decltype(o_isplvm) i_isplvm;

    decltype(o_eser) i_eser;
    decltype(o_eserv) i_eserv;

    decltype(o_espl) i_espl;
    decltype(o_esplv) i_esplv;

    decltype(o_esplm) i_esplm;
    decltype(o_esplvm) i_esplvm;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_iser, i_iserv,
           i_ispl, i_isplv,
           i_isplm, i_isplvm,
           i_eser, i_eserv,
           i_espl, i_esplv,
           i_esplm, i_esplvm );
    }

    BOOST_CHECK(i_iser.x == o_iser.x);
    BOOST_CHECK(i_iserv.x == o_iserv.x);

    BOOST_CHECK(i_ispl.x == o_ispl.x);
    BOOST_CHECK(i_isplv.x == o_isplv.x);

    BOOST_CHECK(i_isplm.x == o_isplm.x);
    BOOST_CHECK(i_isplvm.x == o_isplvm.x);

    BOOST_CHECK(i_eser.x == o_eser.x);
    BOOST_CHECK(i_eserv.x == o_eserv.x);

    BOOST_CHECK(i_espl.x == o_espl.x);
    BOOST_CHECK(i_esplv.x == o_esplv.x);

    BOOST_CHECK(i_esplm.x == o_esplm.x);
    BOOST_CHECK(i_esplvm.x == o_esplvm.x);
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

