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
#ifndef CEREAL_ACCESS_HPP_
#define CEREAL_ACCESS_HPP_

namespace cereal
{
  //! A class that allows cereal to load smart pointers to types that have no default constructor
  /*! If your class does not have a default constructor, cereal will not be able
      to load any smart pointers to it unless you overload LoadAndAllocate
      for your class, and provide an appropriate load_and_allocate method.

      The specialization of LoadAndAllocate must be placed within the cereal namespace:

      @code{.cpp}
      struct MyType
      {
        MyType( int x ); // note: no default ctor
        int myX;

        // Define a serialize or save/load pair as you normally would
        template <class Archive>
        void serialize( Archive & ar )
        {
          ar( myX );
        }
      };

      // Provide a specialization for LoadAndAllocate for your type
      namespace cereal
      {
        template <> struct LoadAndAllocate<MyType>
        {
          // load_and_allocate will be passed the archive that you will be loading
          // from and should return a raw pointer to a dynamically allocated instance
          // of your type.
          //
          // This will be captured by a smart pointer of some type and you need not
          // worry about managing the memory
          template <class Archive>
          static MyType * load_and_allocate( Archive & ar )
          {
            int x;
            ar( x );
            return new MyType( x );
          }
        };
      } // end namespace cereal
      @endcode

      @tparam T The type to specialize for */
  template <class T>
  struct LoadAndAllocate
  {
    //! Called by cereal if no default constructor exists to load and allocate data simultaneously
    /*! Overloads of this should return a pointer to T and expect an archive as a parameter */
    static void load_and_allocate(...)
    { }
  };

  //! A class that can be made a friend to give cereal access to non public functions
  /*! If you desire non-public serialization functions within a class, cereal can only
      access these if you declare cereal::access a friend.

      @code{.cpp}
      class MyClass
      {
        private:
          friend class cereal::access; // gives access to the private serialize

          template <class Archive>
          void serialize( Archive & ar )
          {
            // some code
          }
      };
      @endcode */
  class access
  {
    public:
      template<class Archive, class T> inline
        static auto member_serialize(Archive & ar, T & t) -> decltype(t.serialize(ar))
        { t.serialize(ar); }

      template<class Archive, class T> inline
        static auto member_save(Archive & ar, T const & t) -> decltype(t.save(ar))
        { t.save(ar); }

      template<class Archive, class T> inline
        static auto member_load(Archive & ar, T & t) -> decltype(t.load(ar))
        { t.load(ar); }

      template <class T>
        static void load_and_allocate(...)
        { }

      template<class T, class Archive> inline
        static auto load_and_allocate(Archive & ar) -> decltype(T::load_and_allocate(ar))
        {
          return T::load_and_allocate( ar );
        }
  };
} // namespace cereal

#endif // CEREAL_ACCESS_HPP_
