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

#include <sstream>
#include <stack>
#include <string>

namespace cereal
{
  // ######################################################################
  //! An output archive designed to save data to XML
  class XMLOutputArchive : public OutputArchive<XMLOutputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      XMLOutputArchive(std::ostream & stream) :
        OutputArchive<XMLOutputArchive, AllowEmptyClassElision>(this),
        itsStream(stream),
        itsUnnamedCounter( 0 )
      {
        // rapidxml will delete all allocations when xml_document is cleared
        auto node = itsXML.allocate_node( rapidxml::node_declaration );
        node->append_attribute( itsXML.allocate_attribute( "version", "1.0" ) );
        node->append_attribute( itsXML.allocate_attribute( "encoding", "utf-8" ) );
        itsXML.append_node( node );

        auto root = itsXML.allocate_node( rapidxml::node_element, "cereal" );
        itsXML.append_node( root );
        itsNodes.push( root );
      }

      //! Destructor, flushes the XML
      ~XMLOutputArchive()
      {
        itsStream << itsXML;
        itsXML.clear();
      }

      //! Writes size bytes of data to the output stream
      void saveBinary( const void * data, size_t size )
      {
        size_t const writtenSize = itsStream.rdbuf()->sputn( reinterpret_cast<const char*>( data ), size );

        if(writtenSize != size)
          throw Exception("Failed to write " + std::to_string(size) + " bytes to output stream! Wrote " + std::to_string(writtenSize));
      }

      void createNode( std::string const & name )
      {
        createNode( name.c_str() );
      }

      void createNode( char const * name )
      {
        auto node = itsXML.allocate_node( rapidxml::node_element, name );
        itsNodes.top()->append_node( node );
        itsNodes.push( node );
        ++itsUnnamedCounter;
      }

    private:
      std::ostream & itsStream;
      rapidxml::xml_document<> itsXML; //!< The XML document
      std::stack<rapidxml::xml_node<>*> itsNodes; //!< Stack of nodes, bottom one will always be the root
      size_t itsUnnamedCounter; //!< Used to give names to un-named values
  };

  struct XMLInputArchive;

  //! Serializing NVP types to XML
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT_SERIALIZE(XMLInputArchive, XMLOutputArchive)
  serialize( Archive & ar, NameValuePair<T> & t )
  {
    ar.createNode( t.name );
    //ar( t.value );
  }

  template<class CharT, class Traits, class Alloc> inline
  void save(XMLOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.createNode( str );
  }

  // ######################################################################
  // Common XMLArchive serialization functions
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::XMLOutputArchive);

#endif // CEREAL_ARCHIVES_XML_HPP_
