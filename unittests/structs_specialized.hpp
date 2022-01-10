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
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TEST_STRUCTS_MINIMAL_H_
#define CEREAL_TEST_STRUCTS_MINIMAL_H_
#include "common.hpp"

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

struct BogusBasePolymorphic
{
  template <class Archive>
  void serialize( Archive & ) {}

  virtual void doesNothing() {}

  virtual ~BogusBasePolymorphic() {}
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

class SpecializedMSplitPolymorphic : public BogusBasePolymorphic
{
  public:
    SpecializedMSplitPolymorphic() = default;
    SpecializedMSplitPolymorphic( int xx ) : x(xx) {}

    virtual ~SpecializedMSplitPolymorphic() {}

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
  template <class Archive> struct specialize<Archive, SpecializedMSplitPolymorphic, cereal::specialization::member_load_save> {};

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

CEREAL_REGISTER_TYPE(SpecializedMSplitPolymorphic)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BogusBasePolymorphic, SpecializedMSplitPolymorphic)

template <class IArchive, class OArchive> inline
void test_structs_specialized()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    SpecializedMSerialize               o_iser          = { random_value<int>(gen) };
    SpecializedMSerializeVersioned      o_iserv         = { random_value<int>(gen) };

    SpecializedMSplit                   o_ispl          = { random_value<int>(gen) };
    SpecializedMSplitVersioned          o_isplv         = { random_value<int>(gen) };

    // added re: issue #180
    std::shared_ptr<BogusBasePolymorphic> o_shared_ispl = std::make_shared<SpecializedMSplitPolymorphic>( random_value<int>(gen) );

    SpecializedMSplitMinimal            o_isplm         = { random_value<int>(gen) };
    SpecializedMSplitVersionedMinimal   o_isplvm        = { random_value<int>(gen) };

    SpecializedNMSerialize              o_eser          = { random_value<int>(gen) };
    SpecializedNMSerializeVersioned     o_eserv         = { random_value<int>(gen) };

    SpecializedNMSplit                  o_espl          = { random_value<int>(gen) };
    SpecializedNMSplitVersioned         o_esplv         = { random_value<int>(gen) };

    SpecializedNMSplitMinimal           o_esplm         = { random_value<int>(gen) };
    SpecializedNMSplitVersionedMinimal  o_esplvm        = { random_value<int>(gen) };


    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_iser, o_iserv,
           o_ispl, o_isplv, o_shared_ispl,
           o_isplm, o_isplvm,
           o_eser, o_eserv,
           o_espl, o_esplv,
           o_esplm, o_esplvm );
    }

    decltype(o_iser) i_iser;
    decltype(o_iserv) i_iserv;

    decltype(o_ispl) i_ispl;
    decltype(o_isplv) i_isplv;

    decltype(o_shared_ispl) i_shared_ispl;

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
           i_ispl, i_isplv, i_shared_ispl,
           i_isplm, i_isplvm,
           i_eser, i_eserv,
           i_espl, i_esplv,
           i_esplm, i_esplvm );
    }

    CHECK_EQ(i_iser.x, o_iser.x);
    CHECK_EQ(i_iserv.x, o_iserv.x);

    CHECK_EQ(i_ispl.x, o_ispl.x);
    CHECK_EQ(i_isplv.x, o_isplv.x);

    CHECK_EQ(dynamic_cast<SpecializedMSplitPolymorphic*>(i_shared_ispl.get())->x, dynamic_cast<SpecializedMSplitPolymorphic*>(o_shared_ispl.get())->x);

    CHECK_EQ(i_isplm.x, o_isplm.x);
    CHECK_EQ(i_isplvm.x, o_isplvm.x);

    CHECK_EQ(i_eser.x, o_eser.x);
    CHECK_EQ(i_eserv.x, o_eserv.x);

    CHECK_EQ(i_espl.x, o_espl.x);
    CHECK_EQ(i_esplv.x, o_esplv.x);

    CHECK_EQ(i_esplm.x, o_esplm.x);
    CHECK_EQ(i_esplvm.x, o_esplvm.x);
  }
}

#endif // CEREAL_TEST_STRUCTS_SPECIALIZED_H_
