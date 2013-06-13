#ifndef CEREAL_BINARY_ARCHIVE_BINARY_ARCHIVE_HPP_
#define CEREAL_BINARY_ARCHIVE_BINARY_ARCHIVE_HPP_

#include <cereal/cereal.hpp>

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

    private:
      std::ostream & itsStream;
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
}

#endif // CEREAL_BINARY_ARCHIVE_BINARY_ARCHIVE_HPP_
