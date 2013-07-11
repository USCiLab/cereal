/*! \file access.hpp
    \brief Access control, default construction, and serialization disambiguation */
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
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_ACCESS_HPP_
#define CEREAL_ACCESS_HPP_

#include <type_traits>

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

      @tparam T The type to specialize for
      @ingroup Access */
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
      @endcode
      @ingroup Access */
  class access
  {
    public:
      template<class Archive, class T> inline
        static auto member_serialize(Archive & ar, T & t) -> decltype(t.serialize(ar))
        { t.serialize(ar); }

      template<class Archive, class T> inline
        static auto member_save(Archive & ar, T const & t) -> decltype(t.save(ar))
        { t.save(ar); }

      // Used during detection of non const member save
      template<class Archive, class T> inline
        static auto non_const_member_save(Archive & ar, T & t) -> decltype(t.save(ar))
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

  //! A specifier used in conjunction with cereal::specialize to disambiguate
  //! serialization in special cases
  /*! @relates specialize
      @ingroup Access */
  enum class specialization
  {
    member_serialize,          //!< Force the use of a member serialize function
    member_load_save,          //!< Force the use of a member load/save pair
    non_member_serialize,      //!< Force the use of a non-member serialize function
    non_member_load_save       //!< Force the use of a non-member load/save pair
  };

  //! A class used to disambiguate cases where cereal cannot detect a unique way of serializing a class
  /*! cereal attempts to figure out which method of serialization (member vs. non-member serialize
      or load/save pair) at compile time.  If for some reason cereal cannot find a non-ambiguous way
      of serializing a type, it will produce a static assertion complaining about this.

      This can happen because you have both a serialize and load/save pair, or even because a base
      class has a serialize (public or private with friend access) and a derived class does not
      overwrite this due to choosing some other serialization type.

      Specializing this class will tell cereal to explicitly use the serialization type you specify
      and it will not complain about ambiguity in its compile time selection.  However, if cereal detects
      an ambiguity in specializations, it will continue to issue a static assertion.

      @code{.cpp}
      class MyParent
      {
        friend class cereal::access;
        template <class Archive>
        void serialize( Archive & ar ) {}
      };

      // Although serialize is private in MyParent, to cereal::access it will look public,
      // even through MyDerived
      class MyDerived : public MyParent
      {
        public:
          template <class Archive>
          void load( Archive & ar ) {}

          template <class Archive>
          void save( Archive & ar ) {}
      };

      // The save/load pair in MyDerived is ambiguous because serialize in MyParent can
      // be accessed from cereal::access.  This looks the same as making serialize public
      // in MyParent, making it seem as though MyDerived has both a serialize and a load/save pair.
      // cereal will complain about this at compile time unless we disambiguate:

      namespace cereal
      {
        // This struct specialization will tell cereal which is the right way to serialize the ambiguity
        template <class Archive> struct specialize<Archive, MyDerived, cereal::specialization::member_load_save> {};

        // If we only had a disambiguation for a specific archive type, it would look something like this
        template <> struct specialize<cereal::BinaryOutputArchive, MyDerived, cereal::specialization::member_load_save> {};
      }
      @endcode

      @tparam T The type to specialize the serialization for
      @tparam S The specialization type to use for T
      @ingroup Access */
  template <class Archive, class T, specialization S>
  struct specialize : public std::false_type {};
} // namespace cereal

#endif // CEREAL_ACCESS_HPP_
