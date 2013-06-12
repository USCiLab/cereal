#pragma once

#include <iostream>
#include <type_traits>
#include "traits.hpp"

namespace cereal
{
  namespace detail
  {
    struct NameValuePairCore {};
  }

  //! For holding name value pairs
  template <class T>
  struct NameValuePair : detail::NameValuePairCore
  {
    NameValuePair( std::string const & n, T const & v ) : name(n), value(v) {}

    std::string name;
    T value;
  };

  //! Creates a name value pair
  template <class T> inline
  NameValuePair<T> make_nvp( std::string const & name, T const & value )
  {
    return {name, value};
  }

  //! Creates a name value pair for the variable T, using the same name
  #define CEREAL_NVP(T) ::cereal::make_nvp(#T, T);

  class BinaryOutputArchive
  {
    public:
      BinaryOutputArchive(std::ostream & stream) : itsStream(stream)
      {
      }

      //! Member serialization
      template <class T>
      typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_member_serialize<T, BinaryOutputArchive>(),
               BinaryOutputArchive &>::type
      operator & (T && t)
      {
        std::cout << "Member serialize" << std::endl;

        t.serialize(*this);
        return *this;
      }

      //! Non member serialization
      template <class T>
      typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_non_member_serialize<T, BinaryOutputArchive>(),
               BinaryOutputArchive &>::type
      operator & (T && t)
      {
        std::cout << "Non member serialize" << std::endl;

        serialize(*this, t);
        return *this;
      }

      //! Member split (save)
      template <class T>
      typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_member_split<T, BinaryOutputArchive>(),
               BinaryOutputArchive &>::type
      operator & (T const & t)
      {
        std::cout << "Member split" << std::endl;

        t.save(*this);
        return *this;
      }

      //! Non member split (save)
      template <class T>
      typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_non_member_split<T, BinaryOutputArchive>(),
               BinaryOutputArchive &>::type
      operator & (T const & t)
      {
        std::cout << "Non member split" << std::endl;
        save(*this, t);
        return *this;
      }

      //! No matching serialization
      template <class T>
      typename std::enable_if<!traits::is_serializable<T, BinaryOutputArchive>(), BinaryOutputArchive &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_serializable<T, BinaryOutputArchive>(), "Trying to serialize an unserializable type.\n\n"
            "Types must either have a serialize function, or separate save/load functions (but not both).\n"
            "Serialize functions generally have the following signature:\n\n"
            "template<class Archive>\n"
            "  void serialize(int & ar)\n"
            "  {\n"
            "    ar & member1 & member2 & member3;\n"
            "  }\n\n" );
        return *this;
      }

      //! Writes size bytes of data to the output stream
      void save_binary( const void * data, size_t size )
      {
        auto const writtenSize = itsStream.rdbuf()->sputn( reinterpret_cast<const char*>( data ), size );

        if(writtenSize != size)
          throw 1; // TODO: something terrible
      }

    private:
      std::ostream & itsStream;
  }; // class BinaryOutputArchive

  //! Serialization for POD types to binary
  template<class T>
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  serialize(BinaryOutputArchive & ar, T & t)
  {
    ar.save_binary(std::addressof(t), sizeof(t));
    std::cout << "Serializing POD size: " << sizeof(T) << " [" << t << "]" << std::endl;
  }

  //! Serialization for NVP types to binary
  template<class T>
  typename std::enable_if<std::is_base_of<cereal::detail::NameValuePairCore, T>::value, void>::type
  serialize(BinaryOutputArchive & ar, T & t)
  {
    std::cout << "Serializing NVP: " << t.name << " " << t.value << std::endl;
    ar & t.value;
  }
}
