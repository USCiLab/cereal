/*
  Copyright (c) 2017, Randolph Voorhies, Shane Grant
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
#ifndef CEREAL_TEST_DEFER_H_
#define CEREAL_TEST_DEFER_H_
#include "common.hpp"

struct DeferNode;

struct DeferRelation
{
  std::shared_ptr<DeferNode> node;
  int x;
  std::string y;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( node, x, y );
  }

  bool operator==( DeferRelation const & other ) const;
};

struct DeferNode
{
  DeferNode() = default;
  DeferNode( size_t id_, std::mt19937 & gen ) :
      id(id_),
      w(random_value<long>(gen)),
      iser{random_value<int>(gen), random_value<int>(gen)},
      ispl{random_value<int>(gen), random_value<int>(gen)},
      eser{random_value<int>(gen), random_value<int>(gen)},
      espl{random_value<int>(gen), random_value<int>(gen)},
      relations(),
      z(random_value<char>(gen))
  { }

  size_t id;
  long w;

  StructInternalSerialize iser;
  StructInternalSplit     ispl;
  StructExternalSerialize eser;
  StructExternalSplit     espl;

  std::vector<DeferRelation> relations;

  char z;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( id, w,
        cereal::defer( iser ),
        cereal::defer( ispl ),
        cereal::defer( eser ),
        cereal::defer( espl ),
        cereal::defer( relations ),
        z );
  }

  bool operator==( DeferNode const & other ) const
  {
    bool relationsEqual = true;

    if( relations.size() == other.relations.size() )
      for( size_t i = 0; i < relations.size(); ++i )
        relationsEqual &= relations[i] == other.relations[i];
    else
      relationsEqual = false;

    return id == other.id && w == other.w &&
           iser == other.iser && ispl == other.ispl &&
           eser == other.eser && espl == other.espl &&
           relationsEqual;
  }
};

inline std::ostream& operator<<(std::ostream& os, DeferRelation const & s)
{
  os << "[node(id): " << (s.node ? std::to_string(s.node->id) : "null") << " x: " << s.x << " y: " << s.y << "]";
  return os;
}


bool DeferRelation::operator==( DeferRelation const & other ) const
{
  return x == other.x && y == other.y && node->id == other.node->id;
}


inline std::ostream& operator<<(std::ostream& os, DeferNode const & s)
{
  os << "[id: " << s.id << " w " << s.w << " iser " << s.iser;
  os << " ispl " << s.ispl << " eser " << s.eser << " espl " << s.espl;
  os << " relations (size: " << s.relations.size() << "): ";
  for( auto & r : s.relations )
    os << r;
  os << " z " << s.z << "]";
  return os;
}

template <class IArchive, class OArchive> inline
void test_defer()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    size_t id = 0;
    std::vector<std::shared_ptr<DeferNode>> o_nodes0(1);
    for( auto & node : o_nodes0 )
      node = std::make_shared<DeferNode>(id++, gen);

    std::vector<std::shared_ptr<DeferNode>> o_nodes1(1);
    for( auto & node : o_nodes1 )
      node = std::make_shared<DeferNode>(id++, gen);

    std::shuffle( o_nodes1.begin(), o_nodes1.end(), gen );

    for( auto & node : o_nodes0 )
    {
      node->relations.resize( random_index( 0, 100, gen ) );
      for (auto & r : node->relations)
      {
        r = { o_nodes0[random_index( 0, o_nodes0.size() - 1, gen )],
             random_value<int>( gen ), random_basic_string<char>( gen ) };
      }
    }

    for( auto & node : o_nodes1 )
    {
      node->relations.resize( random_index( 0, 100, gen ) );
      for (auto & r : node->relations)
      {
        r = { o_nodes0[random_index( 0, o_nodes0.size() - 1, gen )],
             random_value<int>( gen ), random_basic_string<char>( gen ) };
      }
    }

    std::ostringstream os;
    {
      OArchive oar(os);

      oar( o_nodes0 );
      oar( o_nodes1 );
      oar.serializeDeferments();
      // TODO: throw exception if deferments not done when destructor fires
    }

    decltype(o_nodes0) i_nodes0;
    decltype(o_nodes1) i_nodes1;

    std::istringstream is(os.str());
    {
      IArchive iar(is);

      iar( i_nodes0 );
      iar( i_nodes1 );
      iar.serializeDeferments();
    }

    check_ptr_collection(o_nodes0, i_nodes0);
    check_ptr_collection(o_nodes1, i_nodes1);
  }
}

#endif // CEREAL_TEST_DEFER_H_
