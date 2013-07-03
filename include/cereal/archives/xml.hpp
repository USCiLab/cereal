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
#include <cereal/details/util.hpp>

#include <cereal/external/rapidxml/rapidxml.hpp>
#include <cereal/external/rapidxml/rapidxml_print.hpp>
#include <cereal/external/base64.hpp>

#include <sstream>
#include <stack>
#include <vector>
#include <string>

#include <iostream>

namespace cereal
{
  // ######################################################################
  //! An output archive designed to save data to XML
  class XMLOutputArchive : public OutputArchive<XMLOutputArchive>
  {
    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout!
          @param precision The precision for floating point output
          @param outputType Controls whether type information will be printed in attributes */
      XMLOutputArchive(std::ostream & stream, size_t precision = 10, bool outputType = false ) :
        OutputArchive<XMLOutputArchive>(this),
        itsStream(stream),
        itsOutputType( outputType )
      {
        // rapidxml will delete all allocations when xml_document is cleared
        auto node = itsXML.allocate_node( rapidxml::node_declaration );
        node->append_attribute( itsXML.allocate_attribute( "version", "1.0" ) );
        node->append_attribute( itsXML.allocate_attribute( "encoding", "utf-8" ) );
        itsXML.append_node( node );

        // allocate root node
        auto root = itsXML.allocate_node( rapidxml::node_element, "cereal" );
        itsXML.append_node( root );
        itsNodes.emplace( root );

        // set attributes on the streams
        itsStream << std::boolalpha;
        itsStream.precision( precision );
        itsOS << std::boolalpha;
        itsOS.precision( precision );
      }

      //! Destructor, flushes the XML
      ~XMLOutputArchive()
      {
        itsStream << itsXML;
        itsXML.clear();
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

        // allocate strings for all of the data in the XML object
        auto dataPtr = itsXML.allocate_string( itsOS.str().c_str() );

        // insert into the XML
        itsNodes.top().node->append_node( itsXML.allocate_node( rapidxml::node_data, nullptr, dataPtr ) );
      }

      //! Creates a new node that is a child of the node at the top of the stack
      /*! Nodes will be given a name that has either been pre-set by a name value pair,
          or generated based upon a counter unique to the parent node.

          The node will then be pushed onto the node stack. */
      void startNode()
      {
        // generate a name for this new node
        const auto nameString = itsNodes.top().getValueName();

        // allocate strings for all of the data in the XML object
        auto namePtr = itsXML.allocate_string( nameString.data(), nameString.size() );

        // insert into the XML
        auto node = itsXML.allocate_node( rapidxml::node_element, namePtr, nullptr, nameString.size() );
        itsNodes.top().node->append_node( node );
        itsNodes.emplace( node );
      }

      //! Causes the type to be appended to the most recently made node if output type is set to true
      template <class T> inline
      void insertType()
      {
        if( !itsOutputType )
          return;

        // generate a name for this new node
        const auto nameString = util::demangledName<T>();

        // allocate strings for all of the data in the XML object
        auto namePtr = itsXML.allocate_string( nameString.data(), nameString.size() );

        itsNodes.top().node->append_attribute( itsXML.allocate_attribute( "type", namePtr ) );
      }

      //! Designates the most recently added node as finished
      void finishNode()
      {
        itsNodes.pop();
      }

      //! Sets the name for the next node created with startNode
      void setNextName( const char * name )
      {
        itsNodes.top().name = name;
      }

      //! Saves some binary data, encoded as a base64 string, with an optional name
      /*! This will create a new node, optionally named, and insert a value that consists of
          the data encoded as a base64 string */
      void saveBinaryValue( const void * data, size_t size, const char * name = nullptr )
      {
        itsNodes.top().name = name;

        startNode();

        auto base64string = base64::encode( reinterpret_cast<const unsigned char *>( data ), size );
        saveValue( base64string );

        if( itsOutputType )
          itsNodes.top().node->append_attribute( itsXML.allocate_attribute( "type", "cereal binary data" ) );

        finishNode();
      };

      //! A struct that contains metadata about a node
      struct NodeInfo
      {
        NodeInfo( rapidxml::xml_node<> * n = nullptr,
                  const char * nm = nullptr ) :
          node( n ),
          counter( 0 ),
          name( nm )
        { }

        rapidxml::xml_node<> * node; //!< A pointer to this node
        size_t counter;              //!< The counter for naming child nodes
        const char * name;           //!< The name for the next child node

        //! Gets the name for the next child node created from this node
        /*! The name will be automatically generated using the counter if
            a name has not been previously set.  If a name has been previously
            set, that name will be returned only once */
        std::string getValueName()
        {
          if( name )
          {
            auto n = name;
            name = nullptr;
            return {n};
          }
          else
            return "value" + std::to_string( counter++ ) + "\0";
        }
      }; // NodeInfo

    private:
      std::ostream & itsStream;
      rapidxml::xml_document<> itsXML; //!< The XML document
      std::stack<NodeInfo> itsNodes;   //!< A stack of nodes added to the document
      std::ostringstream itsOS;        //!< Used to format strings internally
      bool itsOutputType;              //!< Controls whether type information is printed
  }; // XMLOutputArchive

  // ######################################################################
  //! An output archive designed to save data to XML
  class XMLInputArchive : public InputArchive<XMLInputArchive>
  {
    public:
      //! Construct, reading in from the provided stream
      /*! Reads in an entire XML document from some stream and parses it as soon
          as serialization starts

          @param stream The stream to read from.  Can be a stringstream or a file. */
      XMLInputArchive( std::istream & stream ) :
        InputArchive<XMLInputArchive>( this ),
        itsStream( stream ),
        itsData( std::istreambuf_iterator<char>( stream ), std::istreambuf_iterator<char>() )
      {
        try
        {
          itsXML.parse<rapidxml::parse_no_data_nodes | rapidxml::parse_declaration_node>( reinterpret_cast<char *>( itsData.data() ) );
        }
        catch( rapidxml::parse_error const & e )
        {
          throw Exception("XML Parsing failed - likely due to invalid characters or invalid naming");
          //std::cout << e.what() << std::endl;
          //std::cout << e.where<char>() << std::endl;
        }
      }

    //private:
      std::istream & itsStream;
      std::vector<uint8_t> itsData;    //!< The raw data loaded
      rapidxml::xml_document<> itsXML; //!< The XML document
  };

  // ######################################################################
  // XMLArchive prologue and epilogue functions

  //! Prologue for NVPs for XML archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void prologue( XMLOutputArchive &, NameValuePair<T> const & )
  { }

  //! Epilogue for NVPs for XML archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void epilogue( XMLOutputArchive &, NameValuePair<T> const & )
  { }

  //! Prologue for SizeTags for XML archives
  /*! SizeTags are strictly ignored for XML */
  template <class T>
  void prologue( XMLOutputArchive &, SizeTag<T> const & )
  { }

  //! Epilogue for SizeTags for XML archives
  /*! SizeTags are strictly ignored for XML */
  template <class T>
  void epilogue( XMLOutputArchive &, SizeTag<T> const & )
  { }

  //! Prologue for all other types for XML archives
  /*! Starts a new node, named either automatically or by some NVP,
      that may be given data by the type about to be archived */
  template <class T>
  void prologue( XMLOutputArchive & ar, T const & data )
  {
    ar.startNode();
    ar.insertType<T>();
  }

  //! Epilogue for all other types other for XML archives
  /*! Finishes the node created in the prologue */
  template <class T>
  void epilogue( XMLOutputArchive & ar, T const & data )
  {
    ar.finishNode();
  }

  // ######################################################################
  // Common XMLArchive serialization functions

  //! Serializing NVP types to XML
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT_SERIALIZE(XMLInputArchive, XMLOutputArchive)
  serialize( Archive & ar, NameValuePair<T> & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }

  //! Serializing SizeTags to XML
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT_SERIALIZE(XMLInputArchive, XMLOutputArchive)
  serialize( Archive & ar, SizeTag<T> & )
  { }

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

} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::XMLOutputArchive);

#endif // CEREAL_ARCHIVES_XML_HPP_
