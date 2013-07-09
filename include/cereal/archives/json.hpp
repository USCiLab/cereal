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
#include <cereal/external/rapidjson/document.h>
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
    enum class NodeType { StartObject, InObject, StartArray, InArray };

    typedef rapidjson::GenericWriteStream WriteStream;
    typedef rapidjson::PrettyWriter<WriteStream> JSONWriter;

    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout!
          @param precision The precision for floating point output */
      JSONOutputArchive(std::ostream & stream, int precision = 20) :
        OutputArchive<JSONOutputArchive>(this),
        itsWriteStream(stream),
        itsWriter(itsWriteStream, precision)
      {
        itsNameCounter.push(0);
        itsNodeStack.push(NodeType::StartObject);
      }

      //! Destructor, flushes the JSON
      ~JSONOutputArchive()
      {
        itsWriter.EndObject();
      }

      void saveValue(bool b)                { itsWriter.Bool(b);                     }
      void saveValue(int i)                 { itsWriter.Int(i);                      }
      void saveValue(unsigned u)            { itsWriter.Uint(u);                     }
      void saveValue(int64_t i64)           { itsWriter.Int64(i64);                  }
      void saveValue(uint64_t u64)          { itsWriter.Uint64(u64);                 }
      void saveValue(double d)              { itsWriter.Double(d);                   }
      void saveValue(std::string const & s) { itsWriter.String(s.c_str(), s.size()); }
      void saveValue(char const * s)        { itsWriter.String(s);                   }

      //! Save exotic arithmetic types as binary
      template<class T>
        typename std::enable_if<std::is_arithmetic<T>::value &&
                                (sizeof(T) >= sizeof(long double) || sizeof(T) >= sizeof(long long)), void>::type
        saveValue(T const & t)
        {
          auto base64string = base64::encode( reinterpret_cast<const unsigned char *>( &t ), sizeof(T) );
          saveValue( base64string );
        }

      //! Write the name of the upcoming node
      void writeName()
      {
        NodeType const & nodeType = itsNodeStack.top();

        if(nodeType == NodeType::StartArray)
        {
          itsWriter.StartArray();
          itsNodeStack.top() = NodeType::InArray;
        }
        else if(nodeType == NodeType::StartObject)
        {
          itsNodeStack.top() = NodeType::InObject;
          itsWriter.StartObject();
        }

        if(nodeType == NodeType::InArray) return;

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

      void startNode()
      {
        writeName();
        itsNodeStack.push(NodeType::StartObject);
        itsNameCounter.push(0);
      }

      //! Designates the most recently added node as finished
      void finishNode()
      {
        switch(itsNodeStack.top())
        {
          case NodeType::StartArray:
            itsWriter.StartArray();
          case NodeType::InArray:
            itsWriter.EndArray();
            break;
          case NodeType::StartObject:
            itsWriter.StartObject();
          case NodeType::InObject:
            itsWriter.EndObject();
            break;
        }

        itsNodeStack.pop();
        itsNameCounter.pop();
      }

      void makeArray()
      {
        itsNodeStack.top() = NodeType::StartArray;
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
      std::stack<NodeType> itsNodeStack;
  }; // JSONOutputArchive

  // ######################################################################
  //! An input archive designed to load data from JSON
  /*! \note Not working yet!
      \ingroup Archives */
  class JSONInputArchive : public InputArchive<JSONInputArchive>
  {
    typedef rapidjson::GenericReadStream ReadStream;
    typedef rapidjson::GenericValue<rapidjson::UTF8<>> JSONValue;
    typedef JSONValue::ConstMemberIterator JSONIterator;

    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      JSONInputArchive(std::istream & is) :
        InputArchive<JSONInputArchive>(this),
        itsReadStream(is)
      {
        itsDocument.ParseStream<0>(itsReadStream);
        itsValueStack.push(itsDocument.MemberBegin());
      }

      void setNextName(char const * name)
      {
        itsNextName = name;
      }

      void startNode()
      { 
        itsValueStack.push(itsValueStack.top()->value.MemberBegin());
      }

      void finishNode()
      { 
        itsValueStack.pop();
        ++itsValueStack.top();
      }

      void loadValue(bool & val)        { val = itsValueStack.top()->value.GetBool();   ++itsValueStack.top(); }

      template<class T>
        typename std::enable_if<std::is_signed<T>::value && sizeof(T) < sizeof(int64_t), void>::type
        loadValue(T & val)         
        {
          val = itsValueStack.top()->value.GetInt();
          ++itsValueStack.top(); 
        }

      template<class T>
        typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) < sizeof(uint64_t)) &&
                                !std::is_same<bool, T>::value, void>::type
        loadValue(T & val)         
        {
          val = itsValueStack.top()->value.GetUint();
          ++itsValueStack.top(); 
        }

      void loadValue(int64_t & val)     { val = itsValueStack.top()->value.GetInt64();  ++itsValueStack.top(); }
      void loadValue(uint64_t & val)    { val = itsValueStack.top()->value.GetUint64(); ++itsValueStack.top(); }
      void loadValue(float & val)       { val = itsValueStack.top()->value.GetDouble(); ++itsValueStack.top(); }
      void loadValue(double & val)      { val = itsValueStack.top()->value.GetDouble(); ++itsValueStack.top(); }
      void loadValue(std::string & val) { val = itsValueStack.top()->value.GetString(); ++itsValueStack.top(); }

      template<class T>
        typename std::enable_if<std::is_arithmetic<T>::value && 
                                (sizeof(T) >= sizeof(long double) || sizeof(T) >= sizeof(long long)), void>::type
        loadValue(T & val)
        {
          std::string encoded;
          loadValue( encoded );
          auto decoded = base64::decode( encoded );

          if( sizeof(T) != decoded.size() )
            throw Exception("Decoded binary data size does not match specified size");

          std::memcpy( &val, decoded.data(), decoded.size() );
        }

      //! Loads some binary data, encoded as a base64 string, with an optional name
      void loadBinaryValue( void * data, size_t size, char const * name = nullptr)
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

      void loadSize(size_t & size)
      {
        size = itsValueStack.top()->value.Size();
      }


    private:
      char const * itsNextName;
      ReadStream itsReadStream;               //!< Rapidjson write stream
      std::stack<JSONIterator> itsValueStack; //!< Stack of values
      rapidjson::Document itsDocument;        //!< Rapidjson document
  };

  // ######################################################################
  // JSONArchive prologue and epilogue functions
  // ######################################################################

  // ######################################################################
  //! Prologue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void prologue( JSONOutputArchive &, NameValuePair<T> const & )
  { }

  //! Prologue for NVPs for JSON archives
  template <class T>
  void prologue( JSONInputArchive &, NameValuePair<T> const & )
  { }

  // ######################################################################
  //! Epilogue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void epilogue( JSONOutputArchive &, NameValuePair<T> const & )
  { }
  
  //! Epilogue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T>
  void epilogue( JSONInputArchive &, NameValuePair<T> const & )
  { }

  // ######################################################################
  //! Prologue for SizeTags for JSON archives
  /*! SizeTags are strictly ignored for JSON */
  template <class T>
  void prologue( JSONOutputArchive & ar, SizeTag<T> const & )
  { ar.makeArray(); }

  //! Prologue for SizeTags for JSON archives
  template <class T>
  void prologue( JSONInputArchive & ar, SizeTag<T> const & )
  { }

  // ######################################################################
  //! Epilogue for SizeTags for JSON archives
  /*! SizeTags are strictly ignored for JSON */
  template <class T>
  void epilogue( JSONOutputArchive &, SizeTag<T> const & )
  { }

  //! Epilogue for SizeTags for JSON archives
  template <class T>
  void epilogue( JSONInputArchive &, SizeTag<T> const & )
  { }

  // ######################################################################
  //! Prologue for all other types for JSON archives
  /*! Starts a new node, named either automatically or by some NVP,
      that may be given data by the type about to be archived */
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  prologue( JSONOutputArchive & ar, T const & data )
  {
    ar.startNode();
  }

  //! Prologue for all other types for JSON archives
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  prologue( JSONInputArchive & ar, T const & data )
  {
    ar.startNode();
  }

  // ######################################################################
  //! Epilogue for all other types other for JSON archives
  /*! Finishes the node created in the prologue */
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  epilogue( JSONOutputArchive & ar, T const & data )
  {
    ar.finishNode();
  }

  //! Epilogue for all other types other for JSON archives
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  epilogue( JSONInputArchive & ar, T const & data )
  {
    ar.finishNode();
  }

  // ######################################################################
  //! Prologue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( JSONOutputArchive & ar, T const & data )
  {
    ar.writeName();
  }
  
  //! Prologue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( JSONInputArchive & ar, T const & data )
  {
  }

  // ######################################################################
  //! Epilogue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( JSONOutputArchive & ar, T const & data )
  { }

  //! Epilogue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( JSONInputArchive & ar, T const & data )
  { }

  // ######################################################################
  //! Prologue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void prologue(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.writeName();
  }

  //! Prologue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void prologue(JSONInputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
  }


  // ######################################################################
  //! Epilogue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void epilogue(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  { }
  
  //! Epilogue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void epilogue(JSONInputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  { }

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

  //! Saving for arithmetic to JSON
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(JSONOutputArchive & ar, T const & t)
  {
    ar.saveValue( t );
  }

  //! Loading arithmetic from JSON
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load(JSONInputArchive & ar, T & t)
  {
    ar.loadValue( t );
  }

  //! saving string to JSON
  template<class CharT, class Traits, class Alloc> inline
  void save(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.saveValue( str );
  }

  //! loading string from JSON
  template<class CharT, class Traits, class Alloc> inline
  void load(JSONInputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str)
  {
    ar.loadValue( str );
  }

  // ######################################################################
  //! Saving SizeTags to JSON
  template <class T> inline
  void save( JSONOutputArchive &, SizeTag<T> const & )
  { }

  //! Loading SizeTags from JSON
  template <class T> inline
  void load( JSONInputArchive & ar, SizeTag<T> & st )
  {
    ar.loadSize( st.size );
  }

} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::JSONInputArchive);
CEREAL_REGISTER_ARCHIVE(cereal::JSONOutputArchive);

#endif // CEREAL_ARCHIVES_JSON_HPP_
