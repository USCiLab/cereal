#pragma once

namespace cereal
{

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


  //! Serialization for POD types to binary
  template<class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(BinaryOutputArchive & ar, T const & t)
  {
    ar.save_binary(std::addressof(t), sizeof(t));
    std::cout << "Serializing POD size: " << sizeof(T) << " [" << t << "]" << std::endl;
  }

  //! Serialization for NVP types to binary
  template<class T>
  typename std::enable_if<std::is_base_of<cereal::detail::NameValuePairCore, T>::value, void>::type
  save(BinaryOutputArchive & ar, T const & t)
  {
    std::cout << "Serializing NVP: " << t.name << " " << t.value << std::endl;
    ar & t.value;
  }

  //! Serialization for basic_string types to binary
  template<class CharT, class Traits, class Alloc>
  void save(BinaryOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    // Save number of chars + the data
    ar & str.size();
    ar.save_binary(str.data(), str.size() * sizeof(CharT));

    std::cout << "Saving string: " << str << std::endl;
  }

  //! Serialization for basic_string types to binary
  template<class CharT, class Traits, class Alloc>
  void load(BinaryOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str)
  {
    std::cout << "Loading string: " << str << std::endl;
  }
}
