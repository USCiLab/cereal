/*! \file xml.hpp
    \brief XML input and output archives */
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
#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/details/util.hpp>

#include <cereal/external/rapidxml/rapidxml.hpp>
#include <cereal/external/rapidxml/rapidxml_print.hpp>
#include <cereal/external/base64.hpp>

#include <sstream>
#include <stack>
#include <vector>
#include <string>
#include <cstring>

namespace cereal
{
  namespace xml_detail
  {
    static const char * CEREAL_XML_STRING = "cereal";
  }

  // ######################################################################
  //! An output archive designed to save data to XML
  /*! \ingroup Archives */
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
        auto root = itsXML.allocate_node( rapidxml::node_element, xml_detail::CEREAL_XML_STRING );
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

      //! Overload for uint8_t prevents them from being serialized as characters
      void saveValue( uint8_t const & value )
      {
        saveValue( static_cast<uint32_t>( value ) );
      }

      //! Overload for int8_t prevents them from being serialized as characters
      void saveValue( int8_t const & value )
      {
        saveValue( static_cast<int32_t>( value ) );
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

    protected:
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
  /*! \ingroup Archives */
  class XMLInputArchive : public InputArchive<XMLInputArchive>
  {
    public:
      //! Construct, reading in from the provided stream
      /*! Reads in an entire XML document from some stream and parses it as soon
          as serialization starts

          @param stream The stream to read from.  Can be a stringstream or a file. */
      XMLInputArchive( std::istream & stream ) :
        InputArchive<XMLInputArchive>( this ),
        itsData( std::istreambuf_iterator<char>( stream ), std::istreambuf_iterator<char>() )
      {
        try
        {
          itsData.push_back('\0'); // rapidxml will do terrible things without this
          itsXML.parse<rapidxml::parse_no_data_nodes | rapidxml::parse_declaration_node>( reinterpret_cast<char *>( itsData.data() ) );
        }
        catch( rapidxml::parse_error const & e )
        {
          std::cout << "-----Original-----" << std::endl;
          stream.seekg(0);
          std::cout << std::string( std::istreambuf_iterator<char>( stream ), std::istreambuf_iterator<char>() ) << std::endl;

          std::cout << "-----Error-----" << std::endl;
          std::cout << e.what() << std::endl;
          std::cout << e.where<char>() << std::endl;
          throw Exception("XML Parsing failed - likely due to invalid characters or invalid naming");
        }

        // Parse the root
        auto root = itsXML.first_node( xml_detail::CEREAL_XML_STRING );
        itsNodes.emplace( root );
      }

      //! Prepares to start reading the next node
      void startNode()
      {
        itsNodes.emplace( itsNodes.top().child );
      }

      //! Finishes reading the current node
      void finishNode()
      {
        // remove current
        itsNodes.pop();

        // advance parent
        itsNodes.top().advance();
      }

      //! Loads a bool
      template <class T> inline
      typename std::enable_if<std::is_unsigned<T>::value && std::is_same<T, bool>::value, void>::type
      loadValue( T & value )
      {
        std::istringstream is( itsNodes.top().node->value() );
        is.setf( std::ios::boolalpha );
        is >> value;
      }

      template <class T> inline
      typename std::enable_if<std::is_unsigned<T>::value && !std::is_same<T, bool>::value && sizeof(T) < sizeof(long long), void>::type
      loadValue( T & value )
      {
        value = std::stoul( itsNodes.top().node->value() );
      }

      template <class T> inline
      typename std::enable_if<std::is_unsigned<T>::value && !std::is_same<T, bool>::value && sizeof(T) >= sizeof(long long), void>::type
      loadValue( T & value )
      {
        value = std::stoull( itsNodes.top().node->value() );
      }

      template <class T> inline
      typename std::enable_if<std::is_signed<T>::value && sizeof(T) <= sizeof(int), void>::type
      loadValue( T & value )
      {
        value = std::stoi( itsNodes.top().node->value() );
      }

      template <class T> inline
      typename std::enable_if<std::is_signed<T>::value && (sizeof(T) > sizeof(int)) && (sizeof(T) <= sizeof(long)), void>::type
      loadValue( T & value )
      {
        value = std::stol( itsNodes.top().node->value() );
      }

      template <class T> inline
      typename std::enable_if<std::is_signed<T>::value && (sizeof(T) > sizeof(long)) && (sizeof(T) <= sizeof(long long)), void>::type
      loadValue( T & value )
      {
        value = std::stoll( itsNodes.top().node->value() );
      }

      void loadValue( float & value )
      {
        value = std::stof( itsNodes.top().node->value() );
      }

      void loadValue( double & value )
      {
        value = std::stod( itsNodes.top().node->value() );
      }

      //! Loads a string from the current node
      template<class CharT, class Traits, class Alloc> inline
      void loadValue( std::basic_string<CharT, Traits, Alloc> & str )
      {
        std::basic_istringstream<CharT, Traits> is( itsNodes.top().node->value() );

        str.assign( std::istreambuf_iterator<CharT, Traits>( is ),
                    std::istreambuf_iterator<CharT, Traits>() );
      }

      //! Loads some binary data, encoded as a base64 string
      /*! This will automatically start and finish a node to load the data */
      void loadBinaryValue( void * data, size_t size )
      {
        startNode();

        std::string encoded;
        loadValue( encoded );

        auto decoded = base64::decode( encoded );

        if( size != decoded.size() )
          throw Exception("Decoded binary data size does not match specified size");

        std::memcpy( data, decoded.data(), decoded.size() );

        finishNode();
      };

      //! Loads the size of the current node
      template <class T> inline
      void loadSize( T & value )
      {
        value = getNumChildren( itsNodes.top().node );
      }

      //! Gets the number of children (usually interpreted as size) for the specified node
      static size_t getNumChildren( rapidxml::xml_node<> * node = nullptr )
      {
        size_t size = 0;
        node = node->first_node(); // get first child

        while( node != nullptr )
        {
          ++size;
          node = node->next_sibling();
        }

        return size;
      }

    protected:
      //! A struct that contains metadata about a node
      struct NodeInfo
      {
        NodeInfo( rapidxml::xml_node<> * n = nullptr ) :
          node( n ),
          child( n ? n->first_node() : nullptr ),
          size( XMLInputArchive::getNumChildren( n ) )
        { }

        void advance()
        {
          if( size )
          {
            --size;
            child = child->next_sibling();
          }
        }

        rapidxml::xml_node<> * node;  //!< A pointer to this node
        rapidxml::xml_node<> * child; //!< A pointer to its current child
        size_t size;
      }; // NodeInfo

    private:
      std::vector<char> itsData;    //!< The raw data loaded
      rapidxml::xml_document<> itsXML; //!< The XML document
      std::stack<NodeInfo> itsNodes;   //!< A stack of nodes read from the document
  };

  // ######################################################################
  // XMLArchive prologue and epilogue functions
  // ######################################################################

  // ######################################################################
  //! Prologue for NVPs for XML output archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void prologue( XMLOutputArchive &, NameValuePair<T> const & )
  { }

  //! Prologue for NVPs for XML input archives
  template <class T>
  void prologue( XMLInputArchive &, NameValuePair<T> const & )
  { }

  // ######################################################################
  //! Epilogue for NVPs for XML output archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void epilogue( XMLOutputArchive &, NameValuePair<T> const & )
  { }

  //! Epilogue for NVPs for XML input archives
  template <class T>
  void epilogue( XMLInputArchive &, NameValuePair<T> const & )
  { }

  // ######################################################################
  //! Prologue for SizeTags for XML output archives
  /*! SizeTags do not start or finish nodes */
  template <class T>
  void prologue( XMLOutputArchive &, SizeTag<T> const & )
  { }

  template <class T>
  void prologue( XMLInputArchive &, SizeTag<T> const & )
  { }

  //! Epilogue for SizeTags for XML output archives
  /*! SizeTags do not start or finish nodes */
  template <class T>
  void epilogue( XMLOutputArchive &, SizeTag<T> const & )
  { }

  template <class T>
  void epilogue( XMLInputArchive &, SizeTag<T> const & )
  { }

  // ######################################################################
  //! Prologue for all other types for XML output archives
  /*! Starts a new node, named either automatically or by some NVP,
      that may be given data by the type about to be archived */
  template <class T>
  void prologue( XMLOutputArchive & ar, T const & )
  {
    ar.startNode();
    ar.insertType<T>();
  }

  //! Prologue for all other types for XML input archives
  template <class T>
  void prologue( XMLInputArchive & ar, T const & )
  {
    ar.startNode();
  }

  // ######################################################################
  //! Epilogue for all other types other for XML output archives
  /*! Finishes the node created in the prologue */
  template <class T>
  void epilogue( XMLOutputArchive & ar, T const & )
  {
    ar.finishNode();
  }

  //! Epilogue for all other types other for XML output archives
  template <class T>
  void epilogue( XMLInputArchive & ar, T const & )
  {
    ar.finishNode();
  }

  // ######################################################################
  // Common XMLArchive serialization functions
  // ######################################################################

  //! Saving NVP types to XML
  template <class T> inline
  void save( XMLOutputArchive & ar, NameValuePair<T> const & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }

  //! Loading NVP types from XML
  template <class T> inline
  void load( XMLInputArchive & ar, NameValuePair<T> & t )
  {
    ar( t.value );
  }

  // ######################################################################
  //! Saving SizeTags to XML
  template <class T> inline
  void save( XMLOutputArchive &, SizeTag<T> const & )
  { }

  //! Loading SizeTags from XML
  template <class T> inline
  void load( XMLInputArchive & ar, SizeTag<T> & st )
  {
    ar.loadSize( st.size );
  }

  // ######################################################################
  //! Saving for POD types to xml
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(XMLOutputArchive & ar, T const & t)
  {
    ar.saveValue( t );
  }

  //! Loading for POD types from xml
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load(XMLInputArchive & ar, T & t)
  {
    ar.loadValue( t );
  }

  // ######################################################################
  //! saving string to xml
  template<class CharT, class Traits, class Alloc> inline
  void save(XMLOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.saveValue( str );
  }

  //! loading string from xml
  template<class CharT, class Traits, class Alloc> inline
  void load(XMLInputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str)
  {
    ar.loadValue( str );
  }
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::XMLOutputArchive);
CEREAL_REGISTER_ARCHIVE(cereal::XMLInputArchive);

#endif // CEREAL_ARCHIVES_XML_HPP_
