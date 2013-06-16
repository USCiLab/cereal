#ifndef CEREAL_BINARY_ARCHIVE_STACK_HPP_
#define CEREAL_BINARY_ARCHIVE_STACK_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <stack>

namespace cereal
{
  namespace stack_detail
  {
    //! Allows access to the protected container in stack
    template <class T, class C> inline
    C const & container( std::stack<T, C> const & stack )
    {
      struct H : public std::stack<T, C>
      {
        static C const & get( std::stack<T, C> const & s )
        {
          return s.*(&H::c);
        }
      };

      return H::get( stack );
    }
  }

  //! Saving for std::stack to binary
  template <class T, class C> inline
  void save( BinaryOutputArchive & ar, std::stack<T, C> const & stack )
  {
    ar & stack_detail::container( stack );
  }

  //! Loading for std::stack to binary
  template <class T, class C> inline
  void load( BinaryInputArchive & ar, std::stack<T, C> & stack )
  {
    C container;
    ar & container;
    stack = std::stack<T, C>( std::move( container ) );
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_STACK_HPP_
