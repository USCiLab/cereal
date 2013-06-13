#ifndef CEREAL_CEREAL_HPP_
#define CEREAL_CEREAL_HPP_

#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <cstddef>

#include <cereal/details/traits.hpp>

namespace cereal
{
  static const int32_t msb_32bit = 0x80000000;

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
      OutputArchive(ArchiveType * const self) : self(self), itsCurrentPointerId(0)
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

      //! Registers a pointer with the archive
      uint32_t registerSharedPointer( void * addr )
      {
        auto id = itsSharedPointerMap.find( addr );
        if( id == itsSharedPointerMap.end() )
        {
          auto ptrId = itsCurrentPointerId++;
          itsSharedPointerMap.insert( {addr, ptrId} );
          return ptrId | msb_32bit; // mask MSB to be 1
        }
        else
          return id->second;
      }

    private:
      ArchiveType * const self;

      std::unordered_map<void *, std::size_t> itsSharedPointerMap; //!< Maps from addresses to pointer ids
      std::size_t itsCurrentPointerId; //!< The id to be given to the next pointer
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

      std::shared_ptr<void> getSharedPointer(uint32_t const id)
      {
        auto ptr = itsSharedPointerMap.find( id );
        if(ptr == itsSharedPointerMap.end())
        {
          // TODO: Throw a Cereal exception;
          throw std::runtime_error("Error while trying to deserialize a smart pointer. Could not find id " + std::to_string(id));
        }
        return ptr->second;
      }

      void registerSharedPointer(uint32_t const id, std::shared_ptr<void> ptr)
      {
        uint32_t const stripped_id = id & ~msb_32bit;
        itsSharedPointerMap.insert( {stripped_id, ptr} );
      }

    private:
      ArchiveType * const self;
      std::unordered_map<std::size_t, std::shared_ptr<void>> itsSharedPointerMap; //!< Maps from addresses to pointer ids
  }; // class InputArchive
}

#endif // CEREAL_CEREAL_HPP_
