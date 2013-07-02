/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
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
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_ARCHIVES_XML_HPP_
#define CEREAL_ARCHIVES_XML_HPP_

#include <cereal/cereal.hpp>
#include <cereal/external/rapidxml/rapidxml.hpp>
#include <cereal/external/rapidxml/rapidxml_print.hpp>
#include <cereal/external/base64.hpp>

#include <sstream>
#include <stack>
#include <string>

#include <iostream>

namespace cereal
{
  namespace detail
  {
    struct XMLHelper
    {
      const char * popName()
      {
        if( names.empty() )
          return nullptr;
        else
        {
          auto name = names.top();
          names.pop();
          return name;
        }
      }

      std::stack<const char *> names;
    };
  }
  // ######################################################################
  //! An output archive designed to save data to XML
  class XMLOutputArchive : public OutputArchive<XMLOutputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout!
          @param precision The precision for floating point output */
      XMLOutputArchive(std::ostream & stream, size_t precision = 10 ) :
        OutputArchive<XMLOutputArchive, AllowEmptyClassElision>(this),
        itsStream(stream)
      {
        // rapidxml will delete all allocations when xml_document is cleared
        auto node = itsXML.allocate_node( rapidxml::node_declaration );
        node->append_attribute( itsXML.allocate_attribute( "version", "1.0" ) );
        node->append_attribute( itsXML.allocate_attribute( "encoding", "utf-8" ) );
        itsXML.append_node( node );

        auto root = itsXML.allocate_node( rapidxml::node_element, "cereal" );
        itsXML.append_node( root );
        itsNodes.emplace( root );

        // testing
        itsStream << std::boolalpha;
        itsStream.precision( precision );
        itsOS << std::boolalpha;
        itsOS.precision( precision );
        //itsStream.setf( std::ios::floatfield, std::ios::fixed );
      }

      //! Destructor, flushes the XML
      ~XMLOutputArchive()
      {
        itsStream << itsXML;
        itsXML.clear();
      }

      void createNode( std::string const & name )
      {
        createNode( name.c_str() );
      }

      void createNode( char const * name )
      {
        auto node = itsXML.allocate_node( rapidxml::node_element, name );
        itsNodes.top().node->append_node( node );
        itsNodes.emplace( node );

        node->append_node( itsXML.allocate_node( rapidxml::node_data, nullptr, "5432" ) );
      }

      //! Saves some data, encoded as a string
      /*! The data will be be named with the most recent name if one exists,
          otherwise it will be given some default delimited value that depends upon
          the parent node */
      template <class T> inline
      void saveValue( T const & value )
      {
        itsOS.clear(); itsOS.seekp(0);
        itsOS << value << std::ends;
        insertValueNode( itsOS.str().c_str() );
      }

      void insertValueNode( const char * data )
      {
        // allocate strings for all of the data in the XML object
        auto dataPtr = itsXML.allocate_string( data );

        // insert into the XML
        itsNodes.top().node->append_node( itsXML.allocate_node( rapidxml::node_data, nullptr, dataPtr ) );
      }

      void startNode()
      {
        const auto nameString = itsNodes.top().getValueName();

        // allocate strings for all of the data in the XML object
        auto namePtr = itsXML.allocate_string( nameString.data(), nameString.size() );

        // insert into the XML
        auto node = itsXML.allocate_node( rapidxml::node_element, namePtr, nullptr, nameString.size() );
        itsNodes.top().node->append_node( node );
        itsNodes.emplace( node );
      }

      void finishNode()
      {
        itsNodes.pop();
      }

      void setNextName( const char * name )
      {
        itsNodes.top().name = name;
      }

      //! Saves some binary data, encoded as a base64 string
      void saveBinaryValue( const void * data, size_t size )
      {
        auto base64string = base64::encode( reinterpret_cast<const unsigned char *>( data ), size );
        saveValue( base64string );
        //auto decoded = base64::decode(base64string);
        //int const * zz = (int const*)decoded.data();
        //std::cout << zz[0] << " " << zz[1] << " " << zz[2] << std::endl;
      };

      struct NodeInfo
      {
        NodeInfo( rapidxml::xml_node<> * n = nullptr,
                  const char * nm = nullptr ) :
          node( n ),
          counter( 0 ),
          name( nm )
        { }

        rapidxml::xml_node<> * node;
        size_t counter;
        const char * name;

        std::string getValueName()
        {
          if( name )
          {
            auto n = name;
            name = nullptr;
            return {n};
          }
          else
          {
            return "value" + std::to_string( counter++ ) + "\0";
          }
        }
      };

    private:
      std::ostream & itsStream;
      rapidxml::xml_document<> itsXML; //!< The XML document
      std::stack<NodeInfo> itsNodes;
      std::ostringstream itsOS;
  };

  struct XMLInputArchive;

  //! Serializing NVP types to XML
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT_SERIALIZE(XMLInputArchive, XMLOutputArchive)
  serialize( Archive & ar, NameValuePair<T> & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }

  //! Saving for POD types to xml
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(XMLOutputArchive & ar, T const & t)
  {
    ar.saveValue( t );
  }

  //! saving string to xml
  template<class CharT, class Traits, class Alloc> inline
  void save(XMLOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.saveValue( str );
  }

  template <class T>
  void prologue( XMLOutputArchive & ar, NameValuePair<T> const & data )
  { }

  template <class T>
  void epilogue( XMLOutputArchive & ar, NameValuePair<T> const & data )
  { }

  template <class T>
  void prologue( XMLOutputArchive & ar, T const & data )
  {
    ar.startNode();
  }

  template <class T>
  void epilogue( XMLOutputArchive & ar, T const & data )
  {
    ar.finishNode();
  }

  // ######################################################################
  // Common XMLArchive serialization functions
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::XMLOutputArchive);

#endif // CEREAL_ARCHIVES_XML_HPP_
