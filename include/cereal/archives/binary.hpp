/*! \file binary.hpp
    \brief Binary input and output archives */
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
#ifndef CEREAL_ARCHIVES_BINARY_HPP_
#define CEREAL_ARCHIVES_BINARY_HPP_

#include <cereal/cereal.hpp>
#include <sstream>

namespace cereal
{
  // ######################################################################
  //! An output archive designed to save data in a compact binary representation
  /*! This archive outputs data to a stream in an extremely compact binary
      representation with as little extra metadata as possible.

      This archive does nothing to ensure that the endianness of the saved
      and loaded data is the same.  If you need to have portability over
      architectures with different endianness, use PortableBinaryOutputArchive.

      When using a binary archive and a file stream, you must use the
      std::ios::binary format flag to avoid having your data altered
      inadvertently.

      \ingroup Archives */
  template <class Derived>
  class BinaryOutputArchiveBase : public OutputArchive<Derived, AllowEmptyClassElision>
  {
    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      BinaryOutputArchiveBase(Derived * derived, std::ostream & stream) :
        OutputArchive<Derived, AllowEmptyClassElision>(derived),
        itsStream(stream)
      {
        static_assert(std::is_base_of<BinaryOutputArchiveBase, Derived>::value, "The passed class must derive from this one");
        if (static_cast<BinaryOutputArchiveBase *>(derived) != this)
        {
          throw Exception("Wrong derived pointer in BinaryOutputArchiveBase");
        }
      }

      //! Writes size bytes of data to the output stream
      void saveBinary( const void * data, std::size_t size )
      {
        auto const writtenSize = static_cast<std::size_t>( itsStream.rdbuf()->sputn( reinterpret_cast<const char*>( data ), size ) );

        if(writtenSize != size)
          throw Exception("Failed to write " + std::to_string(size) + " bytes to output stream! Wrote " + std::to_string(writtenSize));
      }

      //! Saving for POD types to binary
      template<class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      save_override(T const & t)
      {
        this->saveBinary(std::addressof(t), sizeof(t));
      }

      //! Saving NVP types to binary
      template <class T>
      void save_override( NameValuePair<T> const & t )
      {
        (*this)( t.value );
      }

      //! Saving SizeTags to binary
      template <class T>
      void save_override( SizeTag<T> const & t )
      {
        (*this)( t.size );
      }

      //! Saving binary data
      template <class T>
      void save_override(BinaryData<T> const & bd)
      {
        this->saveBinary( bd.data, static_cast<std::size_t>( bd.size ) );
      }

    private:
      std::ostream & itsStream;
  };

  // ######################################################################
  //! An input archive designed to load data saved using BinaryOutputArchive
  /*  This archive does nothing to ensure that the endianness of the saved
      and loaded data is the same.  If you need to have portability over
      architectures with different endianness, use PortableBinaryOutputArchive.

      When using a binary archive and a file stream, you must use the
      std::ios::binary format flag to avoid having your data altered
      inadvertently.

      \ingroup Archives */

  template <class Derived>
  class BinaryInputArchiveBase : public InputArchive<Derived, AllowEmptyClassElision>
  {
    public:
      //! Construct, loading from the provided stream
      BinaryInputArchiveBase(Derived * derived, std::istream & stream) :
        InputArchive<Derived, AllowEmptyClassElision>(derived),
        itsStream(stream)
      {
        static_assert(std::is_base_of<BinaryInputArchiveBase, Derived>::value, "The passed class must derive from this one");
        if (static_cast<BinaryInputArchiveBase *>(derived) != this)
        {
          throw Exception("Wrong derived pointer in BinaryInputArchiveBase");
        }
      }

      //! Reads size bytes of data from the input stream
      void loadBinary( void * const data, std::size_t size )
      {
        auto const readSize = static_cast<std::size_t>( itsStream.rdbuf()->sgetn( reinterpret_cast<char*>( data ), size ) );

        if(readSize != size)
          throw Exception("Failed to read " + std::to_string(size) + " bytes from input stream! Read " + std::to_string(readSize));
      }

      //! Loading for POD types from binary
      template<class T>
      typename std::enable_if<std::is_arithmetic<T>::value, void>::type
      load_override(T & t)
      {
        this->loadBinary(std::addressof(t), sizeof(t));
      }

      //! Loading for NVP types from binary
      template <class T>
      void load_override( NameValuePair<T> & t )
      {
        (*this)( t.value );
      }

      //! Loading for SizeTags from binary
      template <class T>
      void load_override( SizeTag<T> & t )
      {
        (*this)( t.size );
      }

      //! Loading binary data
      template <class T>
      void load_override(BinaryData<T> & bd)
      {
        this->loadBinary(bd.data, static_cast<std::size_t>(bd.size));
      }

    private:
      std::istream & itsStream;
  };

  class BinaryOutputArchive: public ConcreteArchiveBase<BinaryOutputArchive, BinaryOutputArchiveBase>
  {
    public:
      template <typename... Params>
      BinaryOutputArchive(Params&&... params):
        ConcreteArchiveBase<BinaryOutputArchive, BinaryOutputArchiveBase>(this, std::forward<Params>(params)...)
      {
      }
  };

  class BinaryInputArchive: public ConcreteArchiveBase<BinaryInputArchive, BinaryInputArchiveBase>
  {
    public:
      template <typename... Params>
      BinaryInputArchive(Params&&... params):
        ConcreteArchiveBase<BinaryInputArchive, BinaryInputArchiveBase>(this, std::forward<Params>(params)...)
      {
      }
  };
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::BinaryOutputArchive)
CEREAL_REGISTER_ARCHIVE(cereal::BinaryInputArchive)

#endif // CEREAL_ARCHIVES_BINARY_HPP_
