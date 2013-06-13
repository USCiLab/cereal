#ifndef CEREAL_CEREAL_HPP_
#define CEREAL_CEREAL_HPP_

#include <iostream>
#include <type_traits>

#include <cereal/details/traits.hpp>

namespace cereal
{
  //! For holding name value pairs
  template <class T>
  struct NameValuePair
  {
    NameValuePair( std::string const & n, T & v ) : name(n), value(v) {}

    std::string name;
    T & value;
  };

  //! Creates a name value pair
  template <class T> inline
  NameValuePair<T> make_nvp( std::string const & name, T & value )
  {
    return {name, value};
  }

  //! Creates a name value pair for the variable T, using the same name
  #define CEREAL_NVP(T) ::cereal::make_nvp(#T, T);

  // ######################################################################
  //! The base output archive class
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
        t.serialize(*self);
        return *self;
      }

      //! Non member serialization
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        serialize(*self, std::forward<T>(t));
        return *self;
      }

      //! Member split (save)
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        t.save(*self);
        return *self;
      }

      //! Non member split (save)
      template <class T>
      typename std::enable_if<traits::is_output_serializable<T, ArchiveType>() && traits::has_non_member_save<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T const & t)
      {
        save(*self, t);
        return *self;
      }

      //! No matching serialization
      template <class T>
      typename std::enable_if<!traits::is_output_serializable<T, ArchiveType>(), ArchiveType &>::type
      operator & (T const & t)
      {
        static_assert(traits::is_output_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type with an output archive.\n\n"
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

  // ######################################################################
  //! The base input archive class
  template<class ArchiveType>
  class InputArchive
  {
    public:
      InputArchive(ArchiveType * const self) : self(self) { }

      //! Member serialization
      template <class T>
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        t.serialize(*self);
        return *self;
      }

      //! Non member serialization
      template <class T>
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_serialize<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        serialize(*self, t);
        return *self;
      }

      //! Member split (load)
      template <class T>
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_member_load<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        t.load(*self);
        return *self;
      }

      //! Non member split (load)
      template <class T>
      typename std::enable_if<traits::is_input_serializable<T, ArchiveType>() && traits::has_non_member_load<T, ArchiveType>(),
               ArchiveType &>::type
      operator & (T && t)
      {
        load(*self, std::forward<T>(t));
        return *self;
      }

      //! No matching serialization
      template <class T>
      typename std::enable_if<!traits::is_input_serializable<T, ArchiveType>(), ArchiveType &>::type
      operator & (T & t)
      {
        static_assert(traits::is_input_serializable<T, ArchiveType>(), "Trying to serialize an unserializable type with an input archive.\n\n"
            "Types must either have a serialize function, or separate load/load functions (but not both).\n"
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
  }; // class InputArchive
}

#endif // CEREAL_CEREAL_HPP_
