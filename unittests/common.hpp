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
#ifndef CEREAL_TEST_COMMON_H_
#define CEREAL_TEST_COMMON_H_

#include <cereal/types/memory.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/polymorphic.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <limits>
#include <random>

#include <boost/version.hpp>
#if BOOST_VERSION >= 105900
#include <boost/test/tools/detail/print_helper.hpp>

namespace boost
{
  namespace test_tools
  {
    namespace tt_detail
    {
      template <class F, class S>
      struct print_log_value< ::std::pair<F, S> >
      {
        void operator()(::std::ostream & os, ::std::pair<F, S> const & p )
        {
          os << "([" << p.first << "], [" << p.second << "])";
        }
      };
    }
  }
}

#endif // appropriate boost version

namespace boost
{
  template<class F, class S> inline
  ::std::ostream & operator<<(::std::ostream & os, ::std::pair<F, S> const & p)
  {
    os << "([" << p.first << "], [" << p.second << "])";
    return os;
  }
}

namespace cereal
{
  template <class Archive, class T> inline
    void serialize( Archive &, std::less<T> & )
    { }
}

template<class T> inline
typename std::enable_if<std::is_floating_point<T>::value, T>::type
random_value(std::mt19937 & gen)
{ return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen); }

template<class T> inline
typename std::enable_if<std::is_integral<T>::value && sizeof(T) != sizeof(char), T>::type
random_value(std::mt19937 & gen)
{ return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen); }

template<class T> inline
typename std::enable_if<std::is_integral<T>::value && sizeof(T) == sizeof(char), T>::type
random_value(std::mt19937 & gen)
{ return static_cast<T>( std::uniform_int_distribution<int64_t>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen) ); }

template<class T> inline
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
random_value(std::mt19937 & gen)
{
  std::string s(std::uniform_int_distribution<int>(3, 30)(gen), ' ');
  for(char & c : s)
    c = static_cast<char>( std::uniform_int_distribution<int>( 'A', 'Z' )(gen) );
  return s;
}

template<class C> inline
std::basic_string<C> random_basic_string(std::mt19937 & gen)
{
  std::basic_string<C> s(std::uniform_int_distribution<int>(3, 30)(gen), ' ');
  for(C & c : s)
    c = static_cast<C>( std::uniform_int_distribution<int>( 'A', 'Z' )(gen) );
  return s;
}

template <size_t N> inline
std::string random_binary_string(std::mt19937 & gen)
{
  std::string s(N, ' ');
  for(auto & c : s )
     c = static_cast<char>( std::uniform_int_distribution<int>( '0', '1' )(gen) );
  return s;
}

struct StructBase
{
  StructBase() {}
  StructBase( int xx, int yy ) : x( xx ), y( yy ) {}
  int x, y;
  bool operator==(StructBase const & other) const
  { return x == other.x && y == other.y; }
  bool operator!=(StructBase const & other) const
  { return x != other.x || y != other.y; }
  bool operator<(StructBase const & other) const
  {
    if (x < other.x) return true;
    else if(other.x < x) return false;
    else return (y < other.y);
  }
};

inline std::ostream& operator<<(std::ostream& os, StructBase const & s)
{
    os << "[x: " << s.x << " y: " << s.y << "]";
    return os;
}

struct StructInternalSerialize : StructBase
{
  StructInternalSerialize() : StructBase{0,0} {}
  StructInternalSerialize(int x_, int y_) : StructBase{x_,y_} {}
  template<class Archive>
    void serialize(Archive & ar)
    {
      ar(x, y);
    }
};

struct StructInternalSplit : StructBase
{
  StructInternalSplit() : StructBase{0,0} {}
  StructInternalSplit(int x_, int y_) : StructBase{x_,y_} {}
  template<class Archive>
    void save(Archive & ar) const
    {
      ar(x, y);
    }

  template<class Archive>
    void load(Archive & ar)
    {
      ar(x, y);
    }
};

struct StructExternalSerialize : StructBase
{
  StructExternalSerialize() : StructBase{0,0} {}
  StructExternalSerialize(int x_, int y_) : StructBase{x_,y_} {}
};

  template<class Archive>
void serialize(Archive & ar, StructExternalSerialize & s)
{
  ar(s.x, s.y);
}

struct StructExternalSplit : StructBase
{
  StructExternalSplit() : StructBase{0,0} {}
  StructExternalSplit(int x_, int y_) : StructBase{x_,y_} {}
};

  template<class Archive> inline
void save(Archive & ar, StructExternalSplit const & s)
{
  ar(s.x, s.y);
}

  template<class Archive> inline
void load(Archive & ar, StructExternalSplit & s)
{
  ar(s.x, s.y);
}


template<class T>
struct StructHash {
  public:
    size_t operator()(const T & s) const
    {
      size_t h1 = std::hash<int>()(s.x);
      size_t h2 = std::hash<int>()(s.y);
      return h1 ^ ( h2 << 1 );
    }
};

#endif // CEREAL_TEST_COMMON_H_
