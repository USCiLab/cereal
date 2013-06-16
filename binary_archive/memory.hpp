#ifndef CEREAL_BINARY_ARCHIVE_SHARED_PTR_HPP_
#define CEREAL_BINARY_ARCHIVE_SHARED_PTR_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <memory>

namespace cereal
{
  //! Saving std::shared_ptr to binary
  template <class T> inline
  void save( BinaryOutputArchive & ar, std::shared_ptr<T> const & ptr )
  {
    uint32_t id = ar.registerSharedPointer( ptr.get() );
    ar & id;

    if( id & msb_32bit )
    {
      ar & *ptr;
    }
  }

  //! Loading std::shared_ptr to binary
  template <class T> inline
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

  //! Saving std::weak_ptr to binary
  template <class T> inline
  void save( BinaryOutputArchive & ar, std::weak_ptr<T> const & ptr )
  {
    auto sptr = ptr.lock();
    ar & sptr;
  }

  //! Loading std::weak_ptr from binary
  template <class T> inline
  void load( BinaryInputArchive & ar, std::weak_ptr<T> & ptr )
  {
    std::shared_ptr<T> sptr;
    ar & sptr;
    ptr = sptr;
  }

  //! Saving std::unique_ptr to binary
  template <class T, class D> inline
  void save( BinaryOutputArchive & ar, std::unique_ptr<T, D> const & ptr )
  {
    ar & *ptr;
  }

  //! Loading std::unique_ptr from binary
  template <class T, class D> inline
  void load( BinaryInputArchive & ar, std::unique_ptr<T, D> & ptr )
  {
    ptr.reset(new T);
    ar & *ptr;
  }

} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_SHARED_PTR_HPP_

