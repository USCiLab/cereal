/*! \file message_pack.hpp
    \brief MessagePack input and output archives */
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
#ifndef CEREAL_ARCHIVES_MESSAGE_PACK_HPP_
#define CEREAL_ARCHIVES_MESSAGE_PACK_HPP_

#include <cereal/cereal.hpp>
#include <cereal/details/util.hpp>

#include <cereal/external/msgpack/msgpack.hpp>

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
  class MessagePackOutputArchive : public OutputArchive<MessagePackOutputArchive>
  {
    enum class NodeType { StartObject, InObject, StartArray, InArray };

    public:
      /*! @name Common Functionality
          Common use cases for directly interacting with an MessagePackOutputArchive */
      //! @{

      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.
          @param options The JSON specific options to use.  See the Options struct
                         for the values of default parameters */
      MessagePackOutputArchive(std::ostream & stream) :
        OutputArchive<MessagePackOutputArchive>(this),
        itsWriteStream(stream),
        itsNextName(nullptr)
      {
        itsNameCounter.push(0);
        itsNodeStack.push(NodeType::StartObject);
        itsBufferStack.resize(itsBufferStack.size() + 1);
        itsBufferSize.push(0);
      }

      //! Destructor, flushes the JSON
      ~MessagePackOutputArchive()
      {
        msgpack::packer<std::ostream> packer(itsWriteStream);
        packer.pack_map(itsBufferSize.top()/2);
        packer.pack_bin_body(itsBufferStack.back().data(), itsBufferStack.back().size());
      }

      //! Saves some binary data, encoded as a base64 string, with an optional name
      /*! This will create a new node, optionally named, and insert a value that consists of
          the data encoded as a base64 string */
      void saveBinaryValue( const void * data, size_t size, const char * name = nullptr )
      {
        setNextName( name );
        writeName();
        msgpack::packer<msgpack::sbuffer> packer(itsBufferStack.back());
        packer.pack_bin(size);
        packer.pack_bin_body(static_cast<const char*>(data), size);
        itsBufferSize.top()++;
      };

      //! @}
      /*! @name Internal Functionality
          Functionality designed for use by those requiring control over the inner mechanisms of
          the MessagePackOutputArchive */
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
        itsBufferSize.push(0);
        itsBufferStack.resize(itsBufferStack.size() + 1);
      }

      //! Designates the most recently added node as finished
      void finishNode()
      {
        // if we ended up serializing an empty object or array, writeName
        // will never have been called - so start and then immediately end
        // the object/array.
        //
        // We'll also end any object/arrays we happen to be in

        // Save top node into old buffer
        msgpack::packer<msgpack::sbuffer> packer(*(++itsBufferStack.rbegin()));
        switch(itsNodeStack.top())
        {
           case NodeType::StartArray:
           case NodeType::InArray:
              packer.pack_array(itsBufferSize.top());
              break;
           case NodeType::StartObject:
           case NodeType::InObject:
              packer.pack_map(itsBufferSize.top()/2);
              break;
        }
        packer.pack_bin_body(itsBufferStack.back().data(), itsBufferStack.back().size());
        itsBufferSize.pop();
        itsBufferStack.pop_back();

        itsBufferSize.top()++;
        itsNodeStack.pop();
        itsNameCounter.pop();
      }

      //! Sets the name for the next node created with startNode
      void setNextName( const char * name )
      {
        itsNextName = name;
      }

    public:
      template<class T> inline
      void saveValue(T const & t)
      {
         msgpack::pack(itsBufferStack.back(), t);
         itsBufferSize.top()++;
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
          itsNodeStack.top() = NodeType::InArray;
        }
        else if(nodeType == NodeType::StartObject)
        {
          itsNodeStack.top() = NodeType::InObject;
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
      std::ostream &itsWriteStream;        //!< MessagePack write stream
      char const * itsNextName;            //!< The next name
      std::stack<uint32_t> itsNameCounter; //!< Counter for creating unique names for unnamed nodes
      std::stack<NodeType> itsNodeStack;
      std::list<msgpack::sbuffer> itsBufferStack;
      std::stack<int> itsBufferSize;
  }; // MessagePackOutputArchive



  template<> inline
  void MessagePackOutputArchive::saveValue(long double const & t)
  {
     msgpack::pack(itsBufferStack.back(), static_cast<double>(t));
     itsBufferSize.top()++;
  }


  // ######################################################################
  //! An input archive designed to load data from JSON
  /*! This archive uses RapidJSON to read in a JSON archive.

      Input JSON should have been produced by the MessagePackOutputArchive.  Data can
      only be added to dynamically sized containers (marked by JSON arrays) -
      the input archive will determine their size by looking at the number of child nodes.
      Only JSON originating from a MessagePackOutputArchive is officially supported, but data
      from other sources may work if properly formatted.

      The MessagePackInputArchive does not require that nodes are loaded in the same
      order they were saved by MessagePackOutputArchive.  Using name value pairs (NVPs),
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
  class MessagePackInputArchive : public InputArchive<MessagePackInputArchive>
  {
    public:
      /*! @name Common Functionality
          Common use cases for directly interacting with an MessagePackInputArchive */
      //! @{

      //! Construct, reading from the provided stream
      /*! @param stream The stream to read from */
      MessagePackInputArchive(std::istream & stream) :
        InputArchive<MessagePackInputArchive>(this)
      {
         while(true)
         {
            msgpack::unpacker unpacker;
            unpacker.reserve_buffer();

            auto const readSize = static_cast<std::size_t>( stream.rdbuf()->sgetn( unpacker.buffer(), unpacker.buffer_capacity() ) );
            if (readSize == 0)
            {
               break;
            }

            unpacker.buffer_consumed(readSize);

            if (unpacker.next(itsUnpacked))
            {
               break;
            }
         }

         itsIteratorStack.emplace_back(itsUnpacked.get().via.map.ptr, itsUnpacked.get().via.map.ptr + itsUnpacked.get().via.map.size);
      }

      //! Loads some binary data, encoded as a base64 string
      /*! This will automatically start and finish a node to load the data, and can be called directly by
          users.

          Note that this follows the same ordering rules specified in the class description in regards
          to loading in/out of order */
      void loadBinaryValue( void * data, size_t size, std::string name = std::string() )
      {
        itsNextName = name;

        search();

        if (size != itsIteratorStack.back().value()->via.bin.size)
           throw Exception("Decoded binary data size does not match specified size");

        std::memcpy( data, itsIteratorStack.back().value()->via.bin.ptr, size );

        ++itsIteratorStack.back();
        itsNextName.clear();
      };

    private:
      //! @}
      /*! @name Internal Functionality
          Functionality designed for use by those requiring control over the inner mechanisms of
          the MessagePackInputArchive */
      //! @{

      //! An internal iterator that handles both array and object types
      /*! This class is a variant and holds both types of iterators that
          rapidJSON supports - one for arrays and one for objects. */
      class Iterator
      {
        public:
          Iterator() : itsIndex( 0 ), itsType(Null_) {}

          Iterator(const msgpack::object_kv *begin, const msgpack::object_kv *end) :
            itsMemberItBegin(begin), itsMemberItEnd(end), itsIndex(0), itsType(Member)
          { }

          Iterator(const msgpack::object *begin, const msgpack::object *end) :
            itsValueItBegin(begin), itsValueItEnd(end), itsIndex(0), itsType(Value)
          { }

          //! Advance to the next node
          Iterator & operator++()
          {
            ++itsIndex;
            return *this;
          }

          //! Get the value of the current node
          msgpack::object const * value()
          {
            switch(itsType)
            {
              case Value : return &itsValueItBegin[itsIndex];
              case Member: return &itsMemberItBegin[itsIndex].val;
              default: throw cereal::Exception("Invalid Iterator Type!");
            }
          }

          //! Get the name of the current node, or nullptr if it has no name
          std::string name() const
          {
            if( itsType == Member && (itsMemberItBegin + itsIndex) != itsMemberItEnd )
              return itsMemberItBegin[itsIndex].key.as<std::string>();
            else
              return std::string();
          }

          //! Adjust our position such that we are at the node with the given name
          /*! @throws Exception if no such named node exists */
          inline void search( std::string &searchName )
          {
            size_t index = 0;
            for( auto it = itsMemberItBegin; it != itsMemberItEnd; ++it, ++index )
              if( it->key.as<std::string>() == searchName )
              {
                itsIndex = index;
                return;
              }

            throw Exception("JSON Parsing failed - provided NVP not found");
          }

        private:
          const msgpack::object_kv *itsMemberItBegin, *itsMemberItEnd; //!< The member iterator (object)
          const msgpack::object *itsValueItBegin, *itsValueItEnd;    //!< The value iterator (array)
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
        if( !itsNextName.empty() )
        {
          // The actual name of the current node
          auto const actualName = itsIteratorStack.back().name();

          // Do a search if we don't see a name coming up, or if the names don't match
          if( actualName != itsNextName )
            itsIteratorStack.back().search( itsNextName );
        }

        itsNextName.clear();
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

        if(itsIteratorStack.back().value()->type == msgpack::type::MAP)
          itsIteratorStack.emplace_back(itsIteratorStack.back().value()->via.map.ptr, itsIteratorStack.back().value()->via.map.ptr + itsIteratorStack.back().value()->via.map.size);
        else
          itsIteratorStack.emplace_back(itsIteratorStack.back().value()->via.array.ptr, itsIteratorStack.back().value()->via.array.ptr + itsIteratorStack.back().value()->via.map.size);
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

    public:
      //! Loads a value from the current node
      template<class T> inline
      void loadValue(T & val)
      {
        search();

        val = itsIteratorStack.back().value()->as<T>();
        ++itsIteratorStack.back();
      }

      //! Loads the size for a SizeTag
      void loadSize(size_type & size)
      {
         size = (itsIteratorStack.rbegin() + 1)->value()->via.array.size;
      }

      //! @}

    private:
      msgpack::unpacked itsUnpacked;
      std::string itsNextName;               //!< Next name set by NVP
      std::vector<Iterator> itsIteratorStack; //!< 'Stack' of rapidJSON iterators
  };

  //! Loads a value from the current node
  template<>
  void MessagePackInputArchive::loadValue(long double & val)
  {
    search();

    val = itsIteratorStack.back().value()->as<double>();
    ++itsIteratorStack.back();
  }

  // ######################################################################
  // JSONArchive prologue and epilogue functions
  // ######################################################################

  // ######################################################################
  //! Prologue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T> inline
  void prologue( MessagePackOutputArchive &, NameValuePair<T> const & )
  { }

  //! Prologue for NVPs for JSON archives
  template <class T> inline
  void prologue( MessagePackInputArchive &, NameValuePair<T> const & )
  { }

  // ######################################################################
  //! Epilogue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T> inline
  void epilogue( MessagePackOutputArchive &, NameValuePair<T> const & )
  { }


  //! Epilogue for NVPs for JSON archives
  /*! NVPs do not start or finish nodes - they just set up the names */
  template <class T> inline
  void epilogue( MessagePackInputArchive &, NameValuePair<T> const & )
  { }

  // ######################################################################
  //! Prologue for SizeTags for JSON archives
  /*! SizeTags are strictly ignored for JSON, they just indicate
      that the current node should be made into an array */
  template <class T> inline
  void prologue( MessagePackOutputArchive & ar, SizeTag<T> const & )
  {
    ar.makeArray();
  }


  //! Prologue for SizeTags for JSON archives
  template <class T> inline
  void prologue( MessagePackInputArchive &, SizeTag<T> const & )
  { }

  // ######################################################################
  //! Epilogue for SizeTags for JSON archives
  /*! SizeTags are strictly ignored for JSON */
  template <class T> inline
  void epilogue( MessagePackOutputArchive &, SizeTag<T> const & )
  { }


  //! Epilogue for SizeTags for JSON archives
  template <class T> inline
  void epilogue( MessagePackInputArchive &, SizeTag<T> const & )
  { }

  // ######################################################################
  //! Prologue for all other types for JSON archives (except minimal types)
  /*! Starts a new node, named either automatically or by some NVP,
      that may be given data by the type about to be archived

      Minimal types do not start or finish nodes */
  template <class T> inline
  typename std::enable_if<!std::is_arithmetic<T>::value &&
                          !traits::has_minimal_output_serialization<T, MessagePackOutputArchive>::value, void>::type
  prologue( MessagePackOutputArchive & ar, T const & )
  {
    ar.startNode();
  }

  //! Prologue for all other types for JSON archives
  template <class T> inline
  typename std::enable_if<!std::is_arithmetic<T>::value &&
                       !traits::has_minimal_input_serialization<T, MessagePackOutputArchive>::value, void>::type
  prologue( MessagePackInputArchive & ar, T const & )
  {
    ar.startNode();
  }

  // ######################################################################
  //! Epilogue for all other types other for JSON archives (except minimal types
  /*! Finishes the node created in the prologue

      Minimal types do not start or finish nodes */
  template <class T> inline
  typename std::enable_if<!std::is_arithmetic<T>::value &&
                          !traits::has_minimal_output_serialization<T, MessagePackOutputArchive>::value, void>::type
  epilogue( MessagePackOutputArchive & ar, T const & )
  {
    ar.finishNode();
  }

  //! Epilogue for all other types other for JSON archives
  template <class T> inline
  typename std::enable_if<!std::is_arithmetic<T>::value &&
                       !traits::has_minimal_input_serialization<T, MessagePackOutputArchive>::value, void>::type
  epilogue( MessagePackInputArchive & ar, T const & )
  {
    ar.finishNode();
  }

  // ######################################################################
  //! Prologue for arithmetic types for JSON archives
  template <class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( MessagePackOutputArchive & ar, T const & )
  {
    ar.writeName();
  }


  //! Prologue for arithmetic types for JSON archives
  template <class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  prologue( MessagePackInputArchive &, T const & )
  { }

  // ######################################################################
  //! Epilogue for arithmetic types for JSON archives
  template <class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( MessagePackOutputArchive &, T const & )
  { }


  //! Epilogue for arithmetic types for JSON archives
  template <class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  epilogue( MessagePackInputArchive &, T const & )
  { }

  // ######################################################################
  //! Prologue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void prologue(MessagePackOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const &)
  {
    ar.writeName();
  }


  //! Prologue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void prologue(MessagePackInputArchive &, std::basic_string<CharT, Traits, Alloc> const &)
  { }

  // ######################################################################
  //! Epilogue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void epilogue(MessagePackOutputArchive &, std::basic_string<CharT, Traits, Alloc> const &)
  { }


  //! Epilogue for strings for JSON archives
  template<class CharT, class Traits, class Alloc> inline
  void epilogue(MessagePackInputArchive &, std::basic_string<CharT, Traits, Alloc> const &)
  { }

  // ######################################################################
  // Common JSONArchive serialization functions
  // ######################################################################

  //! Serializing NVP types to JSON
  template <class T> inline
  void save( MessagePackOutputArchive & ar, NameValuePair<T> const & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }


  template <class T> inline
  void load( MessagePackInputArchive & ar, NameValuePair<T> & t )
  {
    ar.setNextName( t.name );
    ar( t.value );
  }

  //! Saving for arithmetic to JSON
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(MessagePackOutputArchive & ar, T const & t)
  {
    ar.saveValue( t );
  }


  //! Loading arithmetic from JSON
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load(MessagePackInputArchive & ar, T & t)
  {
    ar.loadValue( t );
  }

  //! saving string to JSON
  template<class CharT, class Traits, class Alloc> inline
  void save(MessagePackOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    ar.saveValue( str );
  }


  //! loading string from JSON
  template<class CharT, class Traits, class Alloc> inline
  void load(MessagePackInputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str)
  {
    ar.loadValue( str );
  }

  // ######################################################################
  //! Saving SizeTags to JSON
  template <class T> inline
  void save( MessagePackOutputArchive &, SizeTag<T> const & )
  {
    // nothing to do here, we don't explicitly save the size
  }


  //! Loading SizeTags from JSON
  template <class T> inline
  void load( MessagePackInputArchive & ar, SizeTag<T> & st )
  {
    ar.loadSize( st.size );
  }


  //! Saving binary data
  template <class T> inline
  void save(MessagePackOutputArchive & ar, BinaryData<T> const & bd)
  {
    ar.saveBinaryValue( bd.data, static_cast<std::size_t>( bd.size ) );
  }

  //! Loading binary data
  template <class T> inline
  void load(MessagePackInputArchive & ar, BinaryData<T> & bd)
  {
    ar.loadBinaryValue(bd.data, static_cast<std::size_t>(bd.size));
  }

} // namespace cereal

// register archives for polymorphic support

CEREAL_REGISTER_ARCHIVE(cereal::MessagePackInputArchive)
CEREAL_REGISTER_ARCHIVE(cereal::MessagePackOutputArchive)

#endif // CEREAL_ARCHIVES_MESSAGE_PACK_HPP_
