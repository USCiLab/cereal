#ifndef CEREAL_BINARY_ARCHIVE_SHARED_PTR_HPP_
#define CEREAL_BINARY_ARCHIVE_SHARED_PTR_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <memory>

namespace cereal
{
  /*
   *
   *
   *
   *
   *
   *
   *
   *
   *
   *
   */
  //! Saving std::shared_ptr to binary
  template <class T>
  void save( BinaryOutputArchive & ar, std::shared_ptr<T> const & ptr )
  {
    uint32_t id = ar.registerSharedPointer( ptr.get() );
    ar & id;

    std::cout << "ID: " << id << std::endl;

    if( id & msb_32bit )
    {
      std::cout << "Serializing the *ptr" << std::endl;
      ar & *ptr;
    }
  }

  //! Loading std::shared_ptr to binary
  template <class T>
  void load( BinaryInputArchive & ar, std::shared_ptr<T> & ptr )
  {
    uint32_t id;

    ar & id;

    if( id & msb_32bit )
    {
      ptr.reset( new T );
      ar & *ptr;
      ar.registerSharedPointer(id, ptr);
    }
    else
    {
      ptr = std::static_pointer_cast<T>(ar.getSharedPointer(id));
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_SHARED_PTR_HPP_
