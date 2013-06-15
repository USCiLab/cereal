#ifndef CEREAL_BINARY_ARCHIVE_QUEUE_HPP_
#define CEREAL_BINARY_ARCHIVE_QUEUE_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <queue>

namespace cereal
{
  namespace queue_detail
  {
    //! Allows access to the protected container in queue
    template <class T, class C>
    C const & container( std::queue<T, C> const & queue )
    {
      struct H : public std::queue<T, C>
      {
        static C const & get( std::queue<T, C> const & q )
        {
          return q.*(&H::c);
        }
      };

      return H::get( queue );
    }

    //! Allows access to the protected container in priority queue
    template <class T, class C, class Comp>
    C const & container( std::priority_queue<T, C, Comp> const & priority_queue )
    {
      struct H : public std::priority_queue<T, C, Comp>
      {
        static C const & get( std::priority_queue<T, C, Comp> const & pq )
        {
          return pq.*(&H::c);
        }
      };

      return H::get( priority_queue );
    }
    
    //! Allows access to the protected comparator in priority queue
    template <class T, class C, class Comp>
    Comp const & comparator( std::priority_queue<T, C, Comp> const & priority_queue )
    {
      struct H : public std::priority_queue<T, C, Comp>
      {
        static Comp const & get( std::priority_queue<T, C, Comp> const & pq )
        {
          return pq.*(&H::comp);
        }
      };

      return H::get( priority_queue );
    }
  }

  //! Saving for std::queue to binary
  template <class T, class C>
  void save( BinaryOutputArchive & ar, std::queue<T, C> const & queue )
  {
    ar & queue_detail::container( queue );
  }

  //! Loading for std::queue to binary
  template <class T, class C>
  void load( BinaryInputArchive & ar, std::queue<T, C> & queue )
  {
    C container;
    ar & container;
    queue = std::queue<T, C>( std::move( container ) );
  }

  //! Saving for std::priority_queue to binary
  template <class T, class C, class Comp>
  void save( BinaryOutputArchive & ar, std::priority_queue<T, C, Comp> const & priority_queue )
  {
    ar & queue_detail::comparator( priority_queue );
    ar & queue_detail::container( priority_queue );
  }

  //! Loading for std::priority_queue to binary
  template <class T, class C, class Comp>
  void load( BinaryInputArchive & ar, std::priority_queue<T, C, Comp> & priority_queue )
  {
    Comp comparator;
    ar & comparator;
    
    C container;
    ar & container;

    priority_queue = std::priority_queue<T, C, Comp>( comparator, std::move( container ) );
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_QUEUE_HPP_
