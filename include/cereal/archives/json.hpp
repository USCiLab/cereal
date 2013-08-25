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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
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

namespace cereal
{
  // ######################################################################
  //! An output archive designed to save data to JSON
  /*! This archive uses RapidJSON to build serialie data to JSON.

      JSON archives provides a human readable output but at decreased
      performance (both in time and space) compared to binary archives.

      JSON benefits greatly from name-value pairs, which if present, will
      name the nodes in the output.  If these are not present, each level
      of the output will be given an automatically generated delimited name.

      The precision of the output archive controls the number of decimals output
      for floating point numbers and should be sufficiently large (i.e. at least 20)
      if there is a desire to have binary equality between the numbers output and
      those read in.  In general you should expect a loss of precision when going
      from floating point to text and back.

      JSON archives do not output the size information for any dynamically sized structure
      and instead infer it from the number of children for a node.  This means that data
      can be hand edited for dynamic sized structures and will still be readable.  This
      is accomplished through the cereal::SizeTag object, which will cause the archive
      to output the data as a JSON array (e.g. marked by [] instead of {}), which indicates
      that the container is variable sized and may be edited.

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
        itsWriter(itsWriteStream, precision),
        itsNextName(nullptr)
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

      //! Write the name of the upcoming node and prepare object/array state
      /*! Since writeName is called for every value that is output, regardless of
          whether it has a name or not, it is the place where we will do a deferred
          check of our node state and decide whether we are in an array or an object. */
      void writeName()
      {
        NodeType const & nodeType = itsNodeStack.top();

        // Start up either an object or an array, depending on state
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

        // Array types do not output names
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

      //! Starts a new node in the JSON output
      void startNode()
      {
        writeName();
        itsNodeStack.push(NodeType::StartObject);
        itsNameCounter.push(0);
      }

      //! Designates the most recently added node as finished
      void finishNode()
      {
        // if we ended up serializing an empty object or array, writeName
        // will never have been called - so start and then immediately end
        // the object/array.
        //
        // We'll also end any object/arrays we happen to be in
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

      //! Designates that the current node should be output as an array, not an object
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

    private:
      WriteStream itsWriteStream;          //!< Rapidjson write stream
      JSONWriter itsWriter;                //!< Rapidjson writer
      char const * itsNextName;            //!< The next name
      std::stack<uint32_t> itsNameCounter; //!< Counter for creating unique names for unnamed nodes
      std::stack<NodeType> itsNodeStack;
  }; // JSONOutputArchive

  // ######################################################################
  //! An input archive designed to load data from JSON
  /*! This archive uses RapidJSON to read in a JSON archive.

      Input JSON should have been produced by the JSONOutputArchive.  Data can
      only be added to dynamically sized containers (marked by JSON arrays) -
      the input archive will determine their size by looking at the number of child nodes.

      The order of the items in the JSON archive must match what is expected in the
      serialization functions.

      \ingroup Archives */
  class JSONInputArchive : public InputArchive<JSONInputArchive>
  {
    typedef rapidjson::GenericReadStream ReadStream;
    typedef rapidjson::GenericValue<rapidjson::UTF8<>> JSONValue;
    typedef JSONValue::ConstMemberIterator MemberIterator;
    typedef JSONValue::ConstValueIterator ValueIterator;
    typedef rapidjson::Document::GenericValue GenericValue;

    //! An internal iterator that handles both array and object types
    class Iterator
    {
      public:
        Iterator() : itsType(Null) {}

        Iterator(MemberIterator it) :
          itsMemberIt(it), itsType(Member) {}

        Iterator(ValueIterator it) :
          itsValueIt(it), itsType(Value) {}

        Iterator & operator++()
        {
          switch(itsType)
          {
            case Value : ++itsValueIt; break;
            case Member: ++itsMemberIt; break;
            default: throw cereal::Exception("Invalid Iterator Type!");
          }
          return *this;
        }

        GenericValue const & value()
        {
          switch(itsType)
          {
            case Value : return *itsValueIt;
            case Member: return itsMemberIt->value;
            default: throw cereal::Exception("Invalid Iterator Type!");
          }
        }

      private:
        MemberIterator itsMemberIt;
        ValueIterator itsValueIt;
        enum Type {Value, Member, Null} itsType;
    };

    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      JSONInputArchive(std::istream & stream) :
        InputArchive<JSONInputArchive>(this),
        itsReadStream(stream)
      {
        itsDocument.ParseStream<0>(itsReadStream);
        itsValueStack.push_back(itsDocument.MemberBegin());
      }

      //! Starts a new node, going into its proper iterator
      void startNode()
      {
        if(itsValueStack.back().value().IsArray())
          itsValueStack.push_back(itsValueStack.back().value().Begin());
        else
          itsValueStack.push_back(itsValueStack.back().value().MemberBegin());
      }

      //! Finishes the most recently started node
      void finishNode()
      {
        itsValueStack.pop_back();
        ++itsValueStack.back();
      }

      template<class T>
        typename std::enable_if<std::is_signed<T>::value && sizeof(T) < sizeof(int64_t), void>::type
        loadValue(T & val)
        {
          val = itsValueStack.back().value().GetInt();
          ++itsValueStack.back();
        }

      template<class T>
        typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) < sizeof(uint64_t)) &&
                                !std::is_same<bool, T>::value, void>::type
        loadValue(T & val)
        {
          val = itsValueStack.back().value().GetUint();
          ++itsValueStack.back();
        }

      void loadValue(bool & val)        { val = itsValueStack.back().value().GetBool();   ++itsValueStack.back(); }
      void loadValue(int64_t & val)     { val = itsValueStack.back().value().GetInt64();  ++itsValueStack.back(); }
      void loadValue(uint64_t & val)    { val = itsValueStack.back().value().GetUint64(); ++itsValueStack.back(); }
      void loadValue(float & val)       { val = itsValueStack.back().value().GetDouble(); ++itsValueStack.back(); }
      void loadValue(double & val)      { val = itsValueStack.back().value().GetDouble(); ++itsValueStack.back(); }
      void loadValue(std::string & val) { val = itsValueStack.back().value().GetString(); ++itsValueStack.back(); }

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

      //! Loads some binary data, encoded as a base64 string
      void loadBinaryValue( void * data, size_t size )
      {
        std::string encoded;
        loadValue( encoded );
        auto decoded = base64::decode( encoded );

        if( size != decoded.size() )
          throw Exception("Decoded binary data size does not match specified size");

        std::memcpy( data, decoded.data(), decoded.size() );
      };

      //! Loads the size for a SizeTag
      void loadSize(size_type & size)
      {
        size = (itsValueStack.rbegin() + 1)->value().Size();
      }

    private:
      ReadStream itsReadStream;               //!< Rapidjson write stream
      std::vector<Iterator> itsValueStack;    //!< Stack of values
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
  {
    ar.makeArray();
  }

  //! Prologue for SizeTags for JSON archives
  template <class T>
  void prologue( JSONInputArchive &, SizeTag<T> const & )
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
  prologue( JSONOutputArchive & ar, T const & )
  {
    ar.startNode();
  }

  //! Prologue for all other types for JSON archives
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  prologue( JSONInputArchive & ar, T const & )
  {
    ar.startNode();
  }

  // ######################################################################
  //! Epilogue for all other types other for JSON archives
  /*! Finishes the node created in the prologue */
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  epilogue( JSONOutputArchive & ar, T const & )
  {
    ar.finishNode();
  }

  //! Epilogue for all other types other for JSON archives
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
  epilogue( JSONInputArchive & ar, T const & )
  {
    ar.finishNode();
  }

  // ######################################################################
  //! Prologue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( JSONOutputArchive & ar, T const & )
  {
    ar.writeName();
  }

  //! Prologue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( JSONInputArchive &, T const & )
  { }

  // ######################################################################
  //! Epilogue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( JSONOutputArchive &, T const & )
  { }

  //! Epilogue for arithmetic types for JSON archives
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( JSONInputArchive &, T const & )
  { }

  // ######################################################################
  //! Prologue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void prologue(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const &)
  {
    ar.writeName();
  }

  //! Prologue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void prologue(JSONInputArchive &, std::basic_string<CharT, Traits, Alloc> const &)
  { }

  // ######################################################################
  //! Epilogue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void epilogue(JSONOutputArchive &, std::basic_string<CharT, Traits, Alloc> const &)
  { }

  //! Epilogue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void epilogue(JSONInputArchive &, std::basic_string<CharT, Traits, Alloc> const &)
  { }

  // ######################################################################
  // Common JSONArchive serialization functions
  // ######################################################################

  //! Serializing NVP types to JSON
  template <class T> inline
  void save( JSONOutputArchive & ar, NameValuePair<T> const & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }

  template <class T> inline
  void load( JSONInputArchive & ar, NameValuePair<T> & t )
  {
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
