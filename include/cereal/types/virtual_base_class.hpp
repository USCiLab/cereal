/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_VIRTUAL_BASE_CLASS_HPP_
#define CEREAL_TYPES_VIRTUAL_BASE_CLASS_HPP_

namespace cereal
{
  //! Casts a derived class to its base class in a way that allows cereal to track inheritance
  /*! This should be used in cases when a derived type features virtual inheritance from some
      base type.  This allows cereal to track the inheritance and to avoid making duplicate copies
      during serialization.

      It is save to use virtual_base_class in all circumstances for serializing base classes, even in cases
      where virtual inheritance does not take place, though it may be slightly faster to utilize
      static_cast<> if you do not need to worry virtual inheritance

      @code{.cpp}
      struct MyBase
      { };

      struct MyLeft : virtual MyBase
      {
        template <class Archive>
        void serialize( Archive & ar )
        {
          ar( cereal::base_clas<MyBase>( this ) );
        }
      };

      struct MyRight : virtual MyBase
      {
        template <class Archive>
        void serialize( Archive & ar )
        {
          ar( cereal::base_clas<MyBase>( this ) );
        }
      };

      // diamond virtual inheritance; contains one copy if each base class
      struct MyDerived : virtual MyLeft, virtual MyRight
      {
        template <class Archive>
        void serialize( Archive & ar )
        {
          ar( cereal::virtual_base_class<MyLeft>( this ) );  // safely serialize data members in MyLeft
          ar( cereal::virtual_base_class<MyRight>( this ) ); // safely serialize data members in MyRight

          // Because we used virtual_base_class, cereal will ensure that only one instance of MyBase is
          // serialized as we traverse the inheritance heirarchy.

          // If we had chosen to use static_cast<> instead, cereal would perform no tracking and
          // assume that every base class should be serialized (in this case leading to a duplicate
          // serialization of MyBase due to diamond inheritance
      };
      */
  template<class Base>
    struct virtual_base_class
    {
      template<class Derived>
        virtual_base_class(Derived const * derived) :
          base_ptr(const_cast<Base*>(static_cast<Base const *>(derived)))
      { }

        Base * base_ptr;
    };
} // namespace cereal
#endif // CEREAL_TYPES_VIRTUAL_BASE_CLASS_HPP_
