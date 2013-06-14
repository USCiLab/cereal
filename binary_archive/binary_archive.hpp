#ifndef CEREAL_BINARY_ARCHIVE_BINARY_ARCHIVE_HPP_
#define CEREAL_BINARY_ARCHIVE_BINARY_ARCHIVE_HPP_

#include <cereal/cereal.hpp>
#include <stack>

namespace cereal
{
  // ######################################################################
  class BinaryOutputArchive : public OutputArchive<BinaryOutputArchive>
  {
    public:
      BinaryOutputArchive(std::ostream & stream) :
        OutputArchive<BinaryOutputArchive>(this),
        itsStream(stream)
    { }

      //! Writes size bytes of data to the output stream
      void save_binary( const void * data, size_t size )
      {
        auto const writtenSize = itsStream.rdbuf()->sputn( reinterpret_cast<const char*>( data ), size );

        if(writtenSize != size)
          throw 1; // TODO: something terrible
      }

      //! Pushes a placeholder for data onto the archive and saves its position
      void pushPosition( size_t size )
      {
        itsPositionStack.push( itsStream.tellp() );
        for(size_t i = 0; i < size; ++i)
          itsStream.rdbuf()->sputc('\0'); // char doesn't matter, but null-term is zero
      }

      //! Pops the most recently pushed position onto the archive, going to the end
      //! of the archive if the stack is empty
      /*! @return true if the stack is empty and we are at the end of the archive */
      bool popPosition()
      {
        if( itsPositionStack.empty() ) // seek to end of stream
        {
          itsStream.seekp( 0, std::ios_base::end );
          return true;
        }
        else
        {
          itsStream.seekp( itsPositionStack.top() );
          itsPositionStack.pop();
          return false;
        }
      }

      //! Resets the position stack and seeks to the end of the archive
      void resetPosition()
      {
        while( !popPosition() );
      }

    private:
      std::ostream & itsStream;
      std::stack<std::ostream::pos_type> itsPositionStack;
  };

  // ######################################################################
  class BinaryInputArchive : public InputArchive<BinaryInputArchive>
  {
    public:
      BinaryInputArchive(std::istream & stream) :
        InputArchive<BinaryInputArchive>(this),
        itsStream(stream)
    { }

      //! Reads size bytes of data from the input stream
      void load_binary( void * const data, size_t size )
      {
        auto const readSize = itsStream.rdbuf()->sgetn( reinterpret_cast<char*>( data ), size );

        if(readSize != size)
          throw 1; // TODO: something terrible
      }

    private:
      std::istream & itsStream;
  };

  //! Saving for POD types to binary
  template<class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(BinaryOutputArchive & ar, T const & t)
  {
    ar.save_binary(std::addressof(t), sizeof(t));
    //std::cout << "Saving POD size: " << sizeof(T) << " [" << t << "]" << std::endl;
  }

  //! Loading for POD types from binary
  template<class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load(BinaryInputArchive & ar, T & t)
  {
    ar.load_binary(std::addressof(t), sizeof(t));
    //std::cout << "Loading POD size: " << sizeof(T) << " [" << t << "]" << std::endl;
  }

  //! Saving for NVP types to binary
  template<class T>
  void save(BinaryOutputArchive & ar, NameValuePair<T> const & t)
  {
    //std::cout << "Saving NVP: " << t.name << std::endl;
    ar & t.value;
  }

  //! Loading for NVP types from binary
  template<class T>
  void load(BinaryInputArchive & ar, NameValuePair<T> t)
  {
    //std::cout << "Loading NVP... " << std::endl;
    ar & t.value;
  }

  template <class T>
  typename std::enable_if<std::is_array<T>::value, void>::type
  save(BinaryOutputArchive & ar, T const & array)
  {
    ar.save_binary(array, traits::sizeofArray<T>() * sizeof(typename std::remove_all_extents<T>::type));
  }

  template <class T>
  typename std::enable_if<std::is_array<T>::value, void>::type
  load(BinaryInputArchive & ar, T & array)
  {
    ar.load_binary(array, traits::sizeofArray<T>() * sizeof(typename std::remove_all_extents<T>::type));
  }

  template <class Archive, class T>
  void serialize( Archive & ar, T * & t )
  {
    static_assert(!sizeof(T), "Cereal does not support serializing raw pointers - please use a smart pointer");
  }
}

#endif // CEREAL_BINARY_ARCHIVE_BINARY_ARCHIVE_HPP_
