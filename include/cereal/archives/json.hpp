/*! \file json.hpp
    \brief JSON input and output archives */
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
#ifndef CEREAL_ARCHIVES_JSON_HPP_
#define CEREAL_ARCHIVES_JSON_HPP_

#include <cereal/cereal.hpp>
#include <cereal/details/util.hpp>

#include <cereal/external/rapidjson/prettywriter.h>
#include <cereal/external/rapidjson/genericstream.h>
#include <cereal/external/rapidjson/reader.h>
#include <cereal/external/base64.hpp>

#include <sstream>
#include <stack>
#include <vector>
#include <string>

#include <iostream>

namespace cereal
{
  // ######################################################################
  //! An output archive designed to save data to JSON
  /*! \note Not working yet!
      \ingroup Archives */
  class JSONOutputArchive : public OutputArchive<JSONOutputArchive>
  {
    typedef rapidjson::GenericWriteStream WriteStream;
    typedef rapidjson::PrettyWriter<WriteStream> JSONWriter;

    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      JSONOutputArchive(std::ostream & stream) :
        OutputArchive<JSONOutputArchive>(this),
        itsWriteStream(stream),
        itsWriter(itsWriteStream)
      {
        itsNameCounter.push(0);
        itsWriter.StartObject();
      }

      //! Destructor, flushes the JSON
      ~JSONOutputArchive()
      {
        itsWriter.EndObject();
      }

      void saveValue(bool b)                { itsWriter.Bool(b);     }
      void saveValue(int i)                 { itsWriter.Int(i);      }
      void saveValue(unsigned u)            { itsWriter.Uint(u);     }
      void saveValue(int64_t i64)           { itsWriter.Int64(i64);  }
      void saveValue(uint64_t u64)          { itsWriter.Uint64(u64); }
      void saveValue(double d)              { itsWriter.Double(d);   }
      void saveValue(std::string const & s) { rawString(s);          }
      void saveValue(char const * s)        { itsWriter.String(s);   }
      void saveNull()                       { itsWriter.Null();      }

      void writeName()
      {
        if(itsNextName == nullptr)
        {
          std::string name = "value" + std::to_string( itsNameCounter.top()++ ) + "\0";
          saveValue(name);
        }
        else
        {
          saveValue(itsNextName);
          itsNextName = nullptr;
        }
      }

      void rawString(std::string const & s)
      {
        itsWriter.String(s.c_str(), s.size());
      }

      //! Creates a new node that is a child of the node at the top of the stack
      /*! Nodes will be given a name that has either been pre-set by a name value pair,
          or generated based upon a counter unique to the parent node.

          The node will then be pushed onto the node stack. */
      void startNode()
      {
        writeName();

        itsWriter.StartObject();

        itsNameCounter.push(0);
      }

      //! Designates the most recently added node as finished
      void finishNode()
      {
        itsWriter.EndObject();

        itsNameCounter.pop();
      }

      //! Sets the name for the next node created with startNode
      void setNextName( const char * name )
      {
        itsNextName = name;
      }

      //! Saves some binary data, encoded as a base64 string, with an optional name
      /*! This will create a new node, optionally named, and insert a value that consists of
          the data encoded as a base64 string */
      void saveBinaryValue( const void * data, size_t size, const char * name = nullptr )
      {
        setNextName( name );
        writeName();

        auto base64string = base64::encode( reinterpret_cast<const unsigned char *>( data ), size );
        saveValue( base64string );
      };

      void setOutputType(bool outputType)
      {
        itsOutputType = outputType;
      }

    private:
      WriteStream itsWriteStream;          //!< Rapidjson write stream
      JSONWriter itsWriter;                //!< Rapidjson writer
      std::ostringstream itsOS;            //!< Used to format strings internally
      bool itsOutputType;                  //!< Controls whether type information is printed
      char const * itsNextName;            //!< The next name
      std::stack<uint32_t> itsNameCounter; //!< Counter for creating unique names for unnamed nodes
  }; // JSONOutputArchive

  // ######################################################################
  //! An input archive designed to load data from JSON
  /*! \note Not working yet!
      \ingroup Archives */
  class JSONInputArchive : public InputArchive<JSONInputArchive>
  {
    //typedef rapidjson::GenericWriteStream WriteStream;
    //typedef rapidjson::PrettyWriter<WriteStream> JSONWriter;

    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      JSONInputArchive(std::istream & ) :
        InputArchive<JSONInputArchive>(this)
      {
      }
  };

  // ######################################################################
  // JSONArchive prologue and epilogue functions
  // ######################################################################

  //! Prologue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void prologue( JSONOutputArchive &, NameValuePair<T> const & )
  { }

  //! Epilogue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void epilogue( JSONOutputArchive &, NameValuePair<T> const & )
  { }

  //! Prologue for SizeTags for JSON archives
  /*! SizeTags are strictly ignored for JSON */
  template <class T>
  void prologue( JSONOutputArchive &, SizeTag<T> const & )
  { }

  //! Epilogue for SizeTags for JSON archives
  /*! SizeTags are strictly ignored for JSON */
  template <class T>
  void epilogue( JSONOutputArchive &, SizeTag<T> const & )
  { }

  //! Prologue for all other types for JSON archives
  /*! Starts a new node, named either automatically or by some NVP,
      that may be given data by the type about to be archived */
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  prologue( JSONOutputArchive & ar, T const & data )
  {
    ar.startNode();
  }

  //! Epilogue for all other types other for JSON archives
  /*! Finishes the node created in the prologue */
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  epilogue( JSONOutputArchive & ar, T const & data )
  {
    ar.finishNode();
  }

  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( JSONOutputArchive & ar, T const & data )
  {
    ar.writeName();
  }

  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( JSONOutputArchive & ar, T const & data )
  {
  }

  template<class CharT, class Traits, class Alloc> inline
  void prologue(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.writeName();
  }

  template<class CharT, class Traits, class Alloc> inline
  void epilogue(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
  }

  // ######################################################################
  // Common JSONArchive serialization functions
  // ######################################################################

  //! Serializing NVP types to JSON
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(JSONInputArchive, JSONOutputArchive)
  serialize( Archive & ar, NameValuePair<T> & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }

  //! Serializing SizeTags to JSON
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(JSONInputArchive, JSONOutputArchive)
  serialize( Archive &, SizeTag<T> & )
  { }

  //! Saving for arithmetic to JSON
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(JSONOutputArchive & ar, T const & t)
  {
    ar.saveValue( t );
  }

  //! saving string to JSON
  template<class CharT, class Traits, class Alloc> inline
  void save(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.saveValue( str );
  }
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::JSONInputArchive);
CEREAL_REGISTER_ARCHIVE(cereal::JSONOutputArchive);

#endif // CEREAL_ARCHIVES_JSON_HPP_
