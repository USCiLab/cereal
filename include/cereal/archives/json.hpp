/*! \file json.hpp
    \brief JSON input and output archives */
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

namespace cereal
{
  //! An exception thrown when rapidjson fails an internal assertion
  /*! @ingroup Utility */
  struct RapidJSONException : Exception
  { RapidJSONException( const char * what_ ) : Exception( what_ ) {} };
}

// Override rapidjson assertions to throw exceptions by default
#ifndef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x) if(!(x)){ \
  throw ::cereal::RapidJSONException("rapidjson internal assertion failure: " #x); }
#endif // RAPIDJSON_ASSERT

#include <cereal/external/rapidjson/prettywriter.h>
#include <cereal/external/rapidjson/genericstream.h>
#include <cereal/external/rapidjson/reader.h>
#include <cereal/external/rapidjson/document.h>
#include <cereal/external/base64.hpp>

#include <limits>
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
  template <class Derived>
  class JSONOutputArchiveT : public OutputArchive<Derived>
  {
    enum class NodeType { StartObject, InObject, StartArray, InArray };

    typedef rapidjson::GenericWriteStream WriteStream;
    typedef rapidjson::PrettyWriter<WriteStream> JSONWriter;

    public:
      /*! @name Common Functionality
          Common use cases for directly interacting with an JSONOutputArchive */
      //! @{

      //! A class containing various advanced options for the JSON archive
      class Options
      {
        public:
          //! Default options
          static Options Default(){ return Options(); }

          //! Default options with no indentation
          static Options NoIndent(){ return Options( std::numeric_limits<double>::max_digits10, IndentChar::space, 0 ); }

          //! The character to use for indenting
          enum class IndentChar : char
          {
            space = ' ',
            tab = '\t',
            newline = '\n',
            carriage_return = '\r'
          };

          //! Specify specific options for the JSONOutputArchive
          /*! @param precision The precision used for floating point numbers
              @param indentChar The type of character to indent with
              @param indentLength The number of indentChar to use for indentation
                             (0 corresponds to no indentation) */
          explicit Options( int precision = std::numeric_limits<double>::max_digits10,
                            IndentChar indentChar = IndentChar::space,
                            unsigned int indentLength = 4 ) :
            itsPrecision( precision ),
            itsIndentChar( static_cast<char>(indentChar) ),
            itsIndentLength( indentLength ) { }

        private:
          friend class JSONOutputArchiveT;
          int itsPrecision;
          char itsIndentChar;
          unsigned int itsIndentLength;
      };

      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.
          @param options The JSON specific options to use.  See the Options struct
                         for the values of default parameters */
      JSONOutputArchiveT( Derived * derived, std::ostream & stream, Options const & options = Options::Default() ) :
        OutputArchive<Derived>(derived),
        itsWriteStream(stream),
        itsWriter(itsWriteStream, options.itsPrecision),
        itsNextName(nullptr)
      {
        static_assert(std::is_base_of<JSONOutputArchiveT, Derived>::value, "The passed class must derive from this one");
        if (static_cast<JSONOutputArchiveT *>(derived) != this)
        {
          throw Exception("Wrong derived pointer in JSONOutputArchiveT");
        }

        itsWriter.SetIndent( options.itsIndentChar, options.itsIndentLength );
        itsNameCounter.push(0);
        itsNodeStack.push(NodeType::StartObject);
      }

      //! Destructor, flushes the JSON
      ~JSONOutputArchiveT()
      {
        itsWriter.EndObject();
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

      //! @}
      /*! @name Internal Functionality
          Functionality designed for use by those requiring control over the inner mechanisms of
          the JSONOutputArchive */
      //! @{

      //! Starts a new node in the JSON output
      /*! The node can optionally be given a name by calling setNextName prior
          to creating the node

          Nodes only need to be started for types that are themselves objects or arrays */
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

      //! Sets the name for the next node created with startNode
      void setNextName( const char * name )
      {
        itsNextName = name;
      }

      //! Saves a value to the current node - small signed overload
      template<class T> inline
      typename std::enable_if<std::is_signed<T>::value && sizeof(T) < sizeof(int64_t), void>::type
      saveValue(T val)
      {
        itsWriter.Int(val);
      }

      //! Saves a value to the current node - 64 bit signed overload
      //! Note: can't just overload for int64_t - it won't work when both long and long long are 64 bit.
      template<class T> inline
      typename std::enable_if<std::is_signed<T>::value && sizeof(T) == sizeof(int64_t), void>::type
      saveValue(T val)
      {
        itsWriter.Int64(val);
      }

      //! Saves a value to the current node - small unsigned overload
      template<class T> inline
      typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) < sizeof(uint64_t)) &&
                              !std::is_same<bool, T>::value, void>::type
      saveValue(T val)
      {
        itsWriter.Uint(val);
      }
      
      //! Savess a value to the current node - 64 bit unsigned overload
      //! Note: can't just overload for uint64_t - it won't work when both unsigned long and unsigned long long are 64 bit.
      template<class T> inline
      typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) == sizeof(uint64_t)), void>::type
      saveValue(T val)
      {
          itsWriter.Uint64(val);
      }

      //! Saves a bool to the current node
      void saveValue(bool b)                { itsWriter.Bool_(b);                                                        }
      //! Saves a float to the current node
      void saveValue(float f)               { itsWriter.Double(f);                                                       }
      //! Saves a double to the current node
      void saveValue(double d)              { itsWriter.Double(d);                                                       }
      //! Saves a string to the current node
      void saveValue(std::string const & s) { itsWriter.String(s.c_str(), static_cast<rapidjson::SizeType>( s.size() )); }
      //! Saves a const char * to the current node
      void saveValue(char const * s)        { itsWriter.String(s);                                                       }

      //! Prologue for NVPs for JSON archives
      /*! NVPs do not start or finish nodes - they just set up the names */
      template <class T>
      void prologue( NameValuePair<T> const & )
      { }

      //! Epilogue for NVPs for JSON archives
      /*! NVPs do not start or finish nodes - they just set up the names */
      template <class T>
      void epilogue( NameValuePair<T> const & )
      { }

      //! Prologue for SizeTags for JSON archives
      /*! SizeTags are strictly ignored for JSON, they just indicate
          that the current node should be made into an array */
      template <class T>
      void prologue( SizeTag<T> const & )
      {
        this->makeArray();
      }

      //! Epilogue for SizeTags for JSON archives
      /*! SizeTags are strictly ignored for JSON */
      template <class T>
      void epilogue( SizeTag<T> const & )
      { }

      //! Prologue for all other types for JSON archives, except minimal types
      /*! Starts a new node, named either automatically or by some NVP,
          that may be given data by the type about to be archived

          Minimal types do not start or finish nodes */
      template <class T>
      typename std::enable_if<!std::is_arithmetic<T>::value &&
                              !traits::has_minimal_output_serialization<T, Derived>::value, void>::type
      prologue( T const & )
      {
        this->startNode();
      }

      //! Epilogue for all other types other for JSON archives, except minimal types
      /*! Finishes the node created in the prologue

          Minimal types do not start or finish nodes */
      template <class T>
      typename std::enable_if<!std::is_arithmetic<T>::value &&
                              !traits::has_minimal_output_serialization<T, Derived>::value, void>::type
      epilogue( T const & )
      {
        this->finishNode();
      }

      //! Prologue for arithmetic types for JSON archives
      template <class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      prologue( T const & )
      {
        this->writeName();
      }

      //! Epilogue for arithmetic types for JSON archives
      template <class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      epilogue( T const & )
      { }

      //! Prologue for strings for JSON archives
      template<class CharT, class Traits, class Alloc>
      void prologue(std::basic_string<CharT, Traits, Alloc> const &)
      {
        this->writeName();
      }

      //! Epilogue for strings for JSON archives
      template<class CharT, class Traits, class Alloc>
      void epilogue(std::basic_string<CharT, Traits, Alloc> const &)
      { }

      //! Prologue for minimal types
      template <class T>
      typename std::enable_if<traits::has_minimal_output_serialization<T, Derived>::value, void>::type
      prologue( T const & )
      {
      }

      //! Epilogue for minimal types
      template <class T>
      typename std::enable_if<traits::has_minimal_output_serialization<T, Derived>::value, void>::type
      epilogue( T const & )
      {
      }

      //! Serializing NVP types to JSON
      template <class T>
      void save_override( NameValuePair<T> const & t )
      {
        this->setNextName( t.name );
        (*this)( t.value );
      }

      //! Saving for arithmetic to JSON
      template<class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      save_override(T const & t)
      {
        this->saveValue( t );
      }

      //! saving string to JSON
      template<class CharT, class Traits, class Alloc>
      void save_override(std::basic_string<CharT, Traits, Alloc> const & str)
      {
        this->saveValue( str );
      }

      //! Saving SizeTags to JSON
      template <class T>
      void save_override( SizeTag<T> const & )
      {
        // nothing to do here, we don't explicitly save the size
      }

      //! Saves a value to the current node - long double and integers longer than 64-bit
      template<class T> inline
      typename std::enable_if<std::is_arithmetic<T>::value &&
                              !(std::is_integral<T>::value && sizeof(T) <= sizeof(int64_t)) &&
                              !(std::is_floating_point<T>::value && sizeof(T) <= sizeof(double)), void>::type
      saveValue(T val)
      {
        std::stringstream ss; ss.precision( std::numeric_limits<long double>::max_digits10 );
        ss << val;
        saveValue( ss.str() );
      }

      //! Write the name of the upcoming node and prepare object/array state
      /*! Since writeName is called for every value that is output, regardless of
          whether it has a name or not, it is the place where we will do a deferred
          check of our node state and decide whether we are in an array or an object.

          The general workflow of saving to the JSON archive is:

            1. (optional) Set the name for the next node to be created, usually done by an NVP
            2. Start the node
            3. (if there is data to save) Write the name of the node (this function)
            4. (if there is data to save) Save the data (with saveValue)
            5. Finish the node
          */
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

      //! Designates that the current node should be output as an array, not an object
      void makeArray()
      {
        itsNodeStack.top() = NodeType::StartArray;
      }

      //! @}

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
      Only JSON originating from a JSONOutputArchive is officially supported, but data
      from other sources may work if properly formatted.

      The JSONInputArchive does not require that nodes are loaded in the same
      order they were saved by JSONOutputArchive.  Using name value pairs (NVPs),
      it is possible to load in an out of order fashion or otherwise skip/select
      specific nodes to load.

      The default behavior of the input archive is to read sequentially starting
      with the first node and exploring its children.  When a given NVP does
      not match the read in name for a node, the archive will search for that
      node at the current level and load it if it exists.  After loading an out of
      order node, the archive will then proceed back to loading sequentially from
      its new position.

      Consider this simple example where loading of some data is skipped:

      @code{cpp}
      // imagine the input file has someData(1-9) saved in order at the top level node
      ar( someData1, someData2, someData3 );        // XML loads in the order it sees in the file
      ar( cereal::make_nvp( "hello", someData6 ) ); // NVP given does not
                                                    // match expected NVP name, so we search
                                                    // for the given NVP and load that value
      ar( someData7, someData8, someData9 );        // with no NVP given, loading resumes at its
                                                    // current location, proceeding sequentially
      @endcode

      \ingroup Archives */
  template <class Derived>
  class JSONInputArchiveT : public InputArchive<Derived>
  {
    private:
      typedef rapidjson::GenericReadStream ReadStream;
      typedef rapidjson::GenericValue<rapidjson::UTF8<>> JSONValue;
      typedef JSONValue::ConstMemberIterator MemberIterator;
      typedef JSONValue::ConstValueIterator ValueIterator;
      typedef rapidjson::Document::GenericValue GenericValue;

    public:
      /*! @name Common Functionality
          Common use cases for directly interacting with an JSONInputArchive */
      //! @{

      //! Construct, reading from the provided stream
      /*! @param stream The stream to read from */
      JSONInputArchiveT(Derived * derived, std::istream & stream) :
        InputArchive<Derived>(derived),
        itsNextName( nullptr ),
        itsReadStream(stream)
      {
        static_assert(std::is_base_of<JSONInputArchiveT, Derived>::value, "The passed class must derive from this one");
        if (static_cast<JSONInputArchiveT *>(derived) != this)
        {
          throw Exception("Wrong derived pointer in JSONInputArchiveT");
        }

        itsDocument.ParseStream<0>(itsReadStream);
        itsIteratorStack.emplace_back(itsDocument.MemberBegin(), itsDocument.MemberEnd());
      }

      //! Loads some binary data, encoded as a base64 string
      /*! This will automatically start and finish a node to load the data, and can be called directly by
          users.

          Note that this follows the same ordering rules specified in the class description in regards
          to loading in/out of order */
      void loadBinaryValue( void * data, size_t size, const char * name = nullptr )
      {
        itsNextName = name;

        std::string encoded;
        loadValue( encoded );
        auto decoded = base64::decode( encoded );

        if( size != decoded.size() )
          throw Exception("Decoded binary data size does not match specified size");

        std::memcpy( data, decoded.data(), decoded.size() );
        itsNextName = nullptr;
      };

    private:
      //! @}
      /*! @name Internal Functionality
          Functionality designed for use by those requiring control over the inner mechanisms of
          the JSONInputArchive */
      //! @{

      //! An internal iterator that handles both array and object types
      /*! This class is a variant and holds both types of iterators that
          rapidJSON supports - one for arrays and one for objects. */
      class Iterator
      {
        public:
          Iterator() : itsIndex( 0 ), itsType(Null_) {}

          Iterator(MemberIterator begin, MemberIterator end) :
            itsMemberItBegin(begin), itsMemberItEnd(end), itsIndex(0), itsType(Member)
          { }

          Iterator(ValueIterator begin, ValueIterator end) :
            itsValueItBegin(begin), itsValueItEnd(end), itsIndex(0), itsType(Value)
          { }

          //! Advance to the next node
          Iterator & operator++()
          {
            ++itsIndex;
            return *this;
          }

          //! Get the value of the current node
          GenericValue const & value()
          {
            switch(itsType)
            {
              case Value : return itsValueItBegin[itsIndex];
              case Member: return itsMemberItBegin[itsIndex].value;
              default: throw cereal::Exception("Invalid Iterator Type!");
            }
          }

          //! Get the name of the current node, or nullptr if it has no name
          const char * name() const
          {
            if( itsType == Member && (itsMemberItBegin + itsIndex) != itsMemberItEnd )
              return itsMemberItBegin[itsIndex].name.GetString();
            else
              return nullptr;
          }

          //! Adjust our position such that we are at the node with the given name
          /*! @throws Exception if no such named node exists */
          inline void search( const char * searchName )
          {
            const auto len = std::strlen( searchName );
            size_t index = 0;
            for( auto it = itsMemberItBegin; it != itsMemberItEnd; ++it, ++index )
              if( std::strncmp( searchName, it->name.GetString(), len ) == 0 )
              {
                itsIndex = index;
                return;
              }

            throw Exception("JSON Parsing failed - provided NVP not found");
          }

        private:
          MemberIterator itsMemberItBegin, itsMemberItEnd; //!< The member iterator (object)
          ValueIterator itsValueItBegin, itsValueItEnd;    //!< The value iterator (array)
          size_t itsIndex;                                 //!< The current index of this iterator
          enum Type {Value, Member, Null_} itsType;    //!< Whether this holds values (array) or members (objects) or nothing
      };

      //! Searches for the expectedName node if it doesn't match the actualName
      /*! This needs to be called before every load or node start occurs.  This function will
          check to see if an NVP has been provided (with setNextName) and if so, see if that name matches the actual
          next name given.  If the names do not match, it will search in the current level of the JSON for that name.
          If the name is not found, an exception will be thrown.

          Resets the NVP name after called.

          @throws Exception if an expectedName is given and not found */
      inline void search()
      {
        // The name an NVP provided with setNextName()
        if( itsNextName )
        {
          // The actual name of the current node
          auto const actualName = itsIteratorStack.back().name();

          // Do a search if we don't see a name coming up, or if the names don't match
          if( !actualName || std::strcmp( itsNextName, actualName ) != 0 )
            itsIteratorStack.back().search( itsNextName );
        }

        itsNextName = nullptr;
      }

    public:
      //! Starts a new node, going into its proper iterator
      /*! This places an iterator for the next node to be parsed onto the iterator stack.  If the next
          node is an array, this will be a value iterator, otherwise it will be a member iterator.

          By default our strategy is to start with the document root node and then recursively iterate through
          all children in the order they show up in the document.
          We don't need to know NVPs to do this; we'll just blindly load in the order things appear in.

          If we were given an NVP, we will search for it if it does not match our the name of the next node
          that would normally be loaded.  This functionality is provided by search(). */
      void startNode()
      {
        search();

        if(itsIteratorStack.back().value().IsArray())
          itsIteratorStack.emplace_back(itsIteratorStack.back().value().Begin(), itsIteratorStack.back().value().End());
        else
          itsIteratorStack.emplace_back(itsIteratorStack.back().value().MemberBegin(), itsIteratorStack.back().value().MemberEnd());
      }

      //! Finishes the most recently started node
      void finishNode()
      {
        itsIteratorStack.pop_back();
        ++itsIteratorStack.back();
      }

      //! Sets the name for the next node created with startNode
      void setNextName( const char * name )
      {
        itsNextName = name;
      }

      //! Loads a value from the current node - small signed overload
      template<class T> inline
      typename std::enable_if<std::is_signed<T>::value && sizeof(T) < sizeof(int64_t), void>::type
      loadValue(T & val)
      {
        search();

        val = itsIteratorStack.back().value().GetInt();
        ++itsIteratorStack.back();
      }

      //! Loads a value from the current node - 64 bit signed overload
      //! Note: can't just overload for int64_t - it won't work when both long and long long are 64 bit.
      template<class T> inline
      typename std::enable_if<std::is_signed<T>::value && sizeof(T) == sizeof(int64_t), void>::type
      loadValue(T & val)
      {
        search();

        val = itsIteratorStack.back().value().GetInt64();
        ++itsIteratorStack.back();
      }

      //! Loads a value from the current node - small unsigned overload
      template<class T> inline
      typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) < sizeof(uint64_t)) &&
                              !std::is_same<bool, T>::value, void>::type
      loadValue(T & val)
      {
        search();

        val = itsIteratorStack.back().value().GetUint();
        ++itsIteratorStack.back();
      }

      //! Loads a value from the current node - 64 bit unsigned overload
      //! Note: can't just overload for uint64_t - it won't work when both unsigned long and unsigned long long are 64 bit.
      template<class T> inline
      typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) == sizeof(uint64_t)), void>::type
      loadValue(T & val)
      {
          search();
          
          val = itsIteratorStack.back().value().GetUint64();
          ++itsIteratorStack.back();
      }

      //! Loads a value from the current node - bool overload
      void loadValue(bool & val)        { search(); val = itsIteratorStack.back().value().GetBool_();   ++itsIteratorStack.back(); }
      //! Loads a value from the current node - float overload
      void loadValue(float & val)       { search(); val = static_cast<float>(itsIteratorStack.back().value().GetDouble()); ++itsIteratorStack.back(); }
      //! Loads a value from the current node - double overload
      void loadValue(double & val)      { search(); val = itsIteratorStack.back().value().GetDouble(); ++itsIteratorStack.back(); }
      //! Loads a value from the current node - string overload
      void loadValue(std::string & val) { search(); val = itsIteratorStack.back().value().GetString(); ++itsIteratorStack.back(); }

      //! Prologue for NVPs for JSON archives
      /*! NVPs do not start or finish nodes - they just set up the names */
      template <class T>
      void prologue( NameValuePair<T> const & )
      { }

      //! Epilogue for NVPs for JSON archives
      /*! NVPs do not start or finish nodes - they just set up the names */
      template <class T>
      void epilogue( NameValuePair<T> const & )
      { }

      //! Prologue for SizeTags for JSON archives
      /*! SizeTags are strictly ignored for JSON */
      template <class T>
      void prologue( SizeTag<T> const & )
      { }

      //! Epilogue for SizeTags for JSON archives
      /*! SizeTags are strictly ignored for JSON */
      template <class T>
      void epilogue( SizeTag<T> const & )
      { }

      //! Prologue for all other types for JSON archives
      template <class T>
      typename std::enable_if<!std::is_arithmetic<T>::value &&
                              !traits::has_minimal_input_serialization<T, Derived>::value, void>::type
      prologue( T const & )
      {
        this->startNode();
      }

      //! Epilogue for all other types other for JSON archives
      template <class T>
      typename std::enable_if<!std::is_arithmetic<T>::value &&
                              !traits::has_minimal_input_serialization<T, Derived>::value, void>::type
      epilogue( T const & )
      {
        this->finishNode();
      }

      //! Prologue for arithmetic types for JSON archives
      template <class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      prologue( T const & )
      { }

      //! Epilogue for arithmetic types for JSON archives
      template <class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      epilogue( T const & )
      { }

      //! Prologue for strings for JSON archives
      template<class CharT, class Traits, class Alloc>
      void prologue(std::basic_string<CharT, Traits, Alloc> const &)
      { }

      //! Epilogue for strings for JSON archives
      template<class CharT, class Traits, class Alloc>
      void epilogue(std::basic_string<CharT, Traits, Alloc> const &)
      { }

      //! Prologue for minimal types
      template <class T>
      typename std::enable_if<traits::has_minimal_input_serialization<T, Derived>::value, void>::type
      prologue( T const & )
      {
      }

      //! Epilogue for minimal types
      template <class T>
      typename std::enable_if<traits::has_minimal_input_serialization<T, Derived>::value, void>::type
      epilogue( T const & )
      {
      }

      //! Serializing NVP types from JSON
      template <class T>
      void load_override( NameValuePair<T> & t )
      {
        this->setNextName( t.name );
        (*this)( t.value );
      }

      //! Loading arithmetic from JSON
      template<class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      load_override(T & t)
      {
        this->loadValue( t );
      }

      //! loading string from JSON
      template<class CharT, class Traits, class Alloc>
      void load_override(std::basic_string<CharT, Traits, Alloc> & str)
      {
        this->loadValue( str );
      }

      //! Loading SizeTags from JSON
      template <class T>
      void load_override( SizeTag<T> & st )
      {
        this->loadSize( st.size );
      }

    private:
      //! Convert a string to a long long
      void stringToNumber( std::string const & str, long long & val ) { val = std::stoll( str ); }
      //! Convert a string to an unsigned long long
      void stringToNumber( std::string const & str, unsigned long long & val ) { val = std::stoull( str ); }
      //! Convert a string to a long double
      void stringToNumber( std::string const & str, long double & val ) { val = std::stold( str ); }

    public:
      //! Loads a value from the current node - long double and integers longer than 64-bit
      template<class T> inline
      typename std::enable_if<std::is_arithmetic<T>::value &&
                              !(std::is_integral<T>::value && sizeof(T) <= sizeof(int64_t)) &&
                              !(std::is_floating_point<T>::value && sizeof(T) <= sizeof(double)), void>::type
      loadValue(T & val)
      {
        std::string encoded;
        loadValue( encoded );
        stringToNumber( encoded, val );
      }

      //! Loads the size for a SizeTag
      void loadSize(size_type & size)
      {
        size = (itsIteratorStack.rbegin() + 1)->value().Size();
      }

      //! @}

    private:
      const char * itsNextName;               //!< Next name set by NVP
      ReadStream itsReadStream;               //!< Rapidjson write stream
      std::vector<Iterator> itsIteratorStack; //!< 'Stack' of rapidJSON iterators
      rapidjson::Document itsDocument;        //!< Rapidjson document
  };

  using JSONOutputArchive = ConcreteArchive<JSONOutputArchiveT>;
  using JSONInputArchive = ConcreteArchive<JSONInputArchiveT>;
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::JSONInputArchive)
CEREAL_REGISTER_ARCHIVE(cereal::JSONOutputArchive)

#endif // CEREAL_ARCHIVES_JSON_HPP_
