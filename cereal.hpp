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

  template<class ArchiveType>
  class OutputArchive
  {
    public:
      OutputArchive(ArchiveType * const self) : self(self)
      { }

      //! Member serialization
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        std::cout << "Member serialize" << std::endl;

        t.serialize(*self);
        return *self;
      }

      //! Non member serialization
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        std::cout << "Non member serialize" << std::endl;

        serialize(*self, t);
        return *self;
      }

      //! Member split (save)
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        std::cout << "Member split" << std::endl;

        t.save(*self);
        return *self;
      }

      //! Non member split (save)
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        std::cout << "Non member split" << std::endl;
        save(*self, t);
        return *self;
      }

      //! No matching serialization
      template <class T>
      typename std::enable_if<!traits::is_output_serializable<T, ArchiveType>(), ArchiveType &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_output_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type.\n\n"
            "Types must either have a serialize function, or separate save/load functions (but not both).\n"
            "Serialize functions generally have the following signature:\n\n"
            "template<class Archive>\n"
            "  void serialize(int & ar)\n"
            "  {\n"
            "    ar & member1 & member2 & member3;\n"
            "  }\n\n" );
        return *self;
      }

    private:
      ArchiveType * const self;
  }; // class OutputArchive
}
