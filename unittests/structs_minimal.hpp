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

class MemberMinimal
{
  public:
    MemberMinimal() = default;
    MemberMinimal( std::string const & str ) : x( str ) {}

  protected:
    friend class cereal::access;

    template <class Archive>
    std::string save_minimal( Archive const & ) const
    {
      return x;
    }

    template <class Archive>
    void load_minimal( Archive const &, std::string const & str )
    {
      x = str;
    }

  public:
    std::string x;
};

class MemberMinimalVersioned
{
  public:
    MemberMinimalVersioned() = default;
    MemberMinimalVersioned( double d ) : x( d ) {}

  protected:
    friend class cereal::access;

    template <class Archive>
    double save_minimal( Archive const &, const std::uint32_t ) const
    {
      return x;
    }

    template <class Archive>
    void load_minimal( Archive const &, double const & d, const std::uint32_t )
    {
      x = d;
    }

  public:
    double x;
};

struct NonMemberMinimal
{
  NonMemberMinimal() = default;
  NonMemberMinimal( std::uint32_t xx ) : x(xx) {}
  std::uint32_t x;
};

template <class Archive> inline
std::uint32_t save_minimal( Archive const &, NonMemberMinimal const & nmm )
{
  return nmm.x;
}

template <class Archive> inline
void load_minimal( Archive const &, NonMemberMinimal & nmm, std::uint32_t const & data )
{
  nmm.x = data;
}

struct NonMemberMinimalVersioned
{
  NonMemberMinimalVersioned() = default;
  NonMemberMinimalVersioned( bool xx ) : x(xx) {}
  bool x;
};

template <class Archive> inline
bool save_minimal( Archive const &, NonMemberMinimalVersioned const & nmm, std::uint32_t const )
{
  return nmm.x;
}

template <class Archive> inline
void load_minimal( Archive const &, NonMemberMinimalVersioned & nmm, bool const & data, std::uint32_t const )
{
  nmm.x = data;
}

struct TestStruct
{
  TestStruct() = default;
  TestStruct( std::string const & s, double d, std::uint32_t u, bool b ) :
    mm(s), mmv(d), nmm(u), nmmv(b) {}

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( mm, mmv );
    ar( nmm, nmmv );
  }

  MemberMinimal mm;
  MemberMinimalVersioned mmv;
  NonMemberMinimal nmm;
  NonMemberMinimalVersioned nmmv;
};

struct Issue79Struct
{
  Issue79Struct() = default;
  Issue79Struct( std::int32_t xx ) : x(xx) {}
  std::int32_t x;
};

template <class Archive, cereal::traits::DisableIf<std::is_same<Archive, cereal::BinaryOutputArchive>::value ||
                                                   std::is_same<Archive, cereal::PortableBinaryOutputArchive>::value> = cereal::traits::sfinae>
inline std::string save_minimal( Archive const &, Issue79Struct const & val )
{
  return std::to_string( val.x );
}

template <class Archive, cereal::traits::DisableIf<std::is_same<Archive, cereal::BinaryInputArchive>::value ||
                                                   std::is_same<Archive, cereal::PortableBinaryInputArchive>::value> = cereal::traits::sfinae>
inline void load_minimal( Archive const &, Issue79Struct & val, std::string const & str )
{
  val.x = std::stoi( str );
}

template <class Archive, cereal::traits::EnableIf<std::is_same<Archive, cereal::BinaryOutputArchive>::value ||
                                                  std::is_same<Archive, cereal::PortableBinaryOutputArchive>::value> = cereal::traits::sfinae>
inline std::int32_t save_minimal( Archive const &, Issue79Struct const & val )
{
  return val.x;
}

template <class Archive, cereal::traits::EnableIf<std::is_same<Archive, cereal::BinaryInputArchive>::value ||
                                                  std::is_same<Archive, cereal::PortableBinaryInputArchive>::value> = cereal::traits::sfinae>
inline void load_minimal( Archive const &, Issue79Struct & val, std::int32_t const & xx )
{
  val.x = xx;
}

struct Issue79StructInternal
{
  Issue79StructInternal() = default;
  Issue79StructInternal( std::int32_t xx ) : x(xx) {}
  std::int32_t x;

  template <class Archive, cereal::traits::DisableIf<std::is_same<Archive, cereal::BinaryOutputArchive>::value ||
                                                     std::is_same<Archive, cereal::PortableBinaryOutputArchive>::value> = cereal::traits::sfinae>
  inline std::string save_minimal( Archive const & ) const
  {
    return std::to_string( x );
  }

  template <class Archive, cereal::traits::DisableIf<std::is_same<Archive, cereal::BinaryInputArchive>::value ||
                                                     std::is_same<Archive, cereal::PortableBinaryInputArchive>::value> = cereal::traits::sfinae>
  inline void load_minimal( Archive const &, std::string const & str )
  {
    x = std::stoi( str );
  }

  template <class Archive, cereal::traits::EnableIf<std::is_same<Archive, cereal::BinaryOutputArchive>::value ||
                                                    std::is_same<Archive, cereal::PortableBinaryOutputArchive>::value> = cereal::traits::sfinae>
  inline std::int32_t save_minimal( Archive const & ) const
  {
    return x;
  }

  template <class Archive, cereal::traits::EnableIf<std::is_same<Archive, cereal::BinaryInputArchive>::value ||
                                                    std::is_same<Archive, cereal::PortableBinaryInputArchive>::value> = cereal::traits::sfinae>
  inline void load_minimal( Archive const &, std::int32_t const & xx )
  {
    x = xx;
  }
};

template <class IArchive, class OArchive> inline
void test_structs_minimal()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    TestStruct o_struct = { random_basic_string<char>(gen), random_value<double>(gen),
                            random_value<std::uint32_t>(gen), random_value<uint8_t>(gen) % 2 ? true : false };

    Issue79Struct o_struct2 = { random_value<std::int32_t>(gen) };
    Issue79StructInternal o_struct3 = { random_value<std::int32_t>(gen) };

    std::ostringstream os;
    {
      OArchive oar(os);
      oar( o_struct );
      oar( o_struct2 );
      oar( o_struct3 );
    }

    decltype(o_struct) i_struct;
    decltype(o_struct2) i_struct2;
    decltype(o_struct3) i_struct3;

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      iar( i_struct );
      iar( i_struct2 );
      iar( i_struct3 );
    }

    CHECK_EQ(o_struct.mm.x, i_struct.mm.x);
    CHECK_EQ(o_struct.mmv.x, doctest::Approx(i_struct.mmv.x).epsilon(1e-5));

    CHECK_EQ(o_struct.nmm.x, i_struct.nmm.x);
    CHECK_EQ(o_struct.nmmv.x, i_struct.nmmv.x);

    CHECK_EQ(o_struct2.x, i_struct2.x);

    CHECK_EQ(o_struct3.x, i_struct3.x);
  }
}

#endif // CEREAL_TEST_STRUCTS_MINIMAL_H_
