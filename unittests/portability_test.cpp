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

#include <cereal/archives/portable_binary.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>

#include <fstream>
#include <iostream>

struct Data : std::enable_shared_from_this<Data>
{
  int32_t x;
  int64_t y;

  Data( int32_t xx, int64_t yy ) : x(xx), y(yy) {}

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x, y );
  }

  template <class Archive>
  static void load_and_construct( Archive & ar, cereal::construct<Data> & construct )
  {
    int32_t xx;
    int64_t yy;
    ar( xx, yy );
    construct( xx, yy );
  }

  bool operator==( Data const & other ) const
  {
    if( x != other.x )
    {
      std::cerr << "x=" << x << ", other.x=" << other.x << std::endl;
      return false;
    }
    if( y != other.y )
    {
      std::cerr << "y=" << y << ", other.y=" << other.y << std::endl;
      return false;
    }

    return true;
  }
};

struct Data2
{
  int32_t x;
  int64_t y;

  Data2() = default;
  Data2( int32_t xx, int64_t yy ) : x(xx), y(yy) {}

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x, y );
  }

  bool operator==( Data2 const & other ) const
  {
    if( x != other.x )
    {
      std::cerr << "x=" << x << ", other.x=" << other.x << std::endl;
      return false;
    }
    if( y != other.y )
    {
      std::cerr << "y=" << y << ", other.y=" << other.y << std::endl;
      return false;
    }

    return true;
  }
};

const uint32_t AnotherCount = 32;

struct Another
{
  Another() = default;

  Another( bool )
  {
    for( uint32_t i = 0; i < AnotherCount; ++i )
      data[i] = Data2( i, i+1 );
  }

  std::map<std::uint32_t, Data2> data;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( data );
  }

  bool operator==( Another const & other ) const
  {
    auto iter  = data.begin();
    auto oiter = other.data.begin();
    for( ; iter != data.end(); ++iter, ++oiter )
    {
      if( !(iter->second == oiter->second) )
      {
        std::cerr << "Data mismatch at i=" << iter->first << std::endl;
        return false;
      }
    }

    return true;
  }
};

int main( int, char ** argv )
{
  std::vector<Another> vec_o;
  for( int i = 0; i < 5; ++i )
    vec_o.emplace_back( true );

  auto data_o = std::make_shared<Data>( 33, 64 );
  int32_t int_o = 7;

  std::cerr << "Portability test: " << argv[2] << "bit" << std::endl;

  if( std::string(argv[1]) == "load" )
  {
    std::ifstream is("portable.cereal", std::ios::binary);
    cereal::PortableBinaryInputArchive ar( is );

    std::vector<Another> vec_i;
    std::shared_ptr<Data> data_i;
    int32_t int_i;

    ar( int_i );

    if( int_i != int_o )
    {
      std::cerr << "in " << int_i << ", out: " << int_o << std::endl;
      return -1;
    }

    ar( vec_i );
    ar( data_i );

    if( vec_i != vec_o )
    {
      std::cerr << "Input vector did not equal output vector" << std::endl;
      return -1;
    }

    if( !(*data_i == *data_o) )
    {
      std::cerr << "Data did not match" << std::endl;
      return -1;
    }
  }
  else if( std::string(argv[1]) == "save" )
  {
    std::ofstream os("portable.cereal", std::ios::binary);
    cereal::PortableBinaryOutputArchive ar( os );

    ar( int_o );
    ar( vec_o );
    ar( data_o );
  }
  else // clean
  {
    std::remove( "portable.cereal" );
  }

  return 0;
}
