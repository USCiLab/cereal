#pragma once

#include <iostream>
#include <type_traits>
#include "traits.hpp"

namespace cereal
{

  class BinaryOutputArchive
  {
    public:

      BinaryOutputArchive() {}

      BinaryOutputArchive(std::ostream & stream)
      {
      }

      template<class T>
        typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_member_serialize<T, BinaryOutputArchive>(),
                 BinaryOutputArchive &>::type
                   operator & (T const & t)
                   { 
                     //t.serialize(*this, traits::version<T>::value)
                     return *this;
                   }

      template<class T>
        typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_non_member_serialize<T, BinaryOutputArchive>(),
                 BinaryOutputArchive &>::type
                   operator & (T const & t)
                   { 
                     //serialize(*this, t, traits::version<T>::value)
                     return *this;
                   }

      template<class T>
        typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_member_split<T, BinaryOutputArchive>(),
                 BinaryOutputArchive &>::type
                   operator & (T const & t)
                   { 
                     //t.save(*this, traits::version<T>::value);
                     return *this;
                   }

      template<class T>
        typename std::enable_if<traits::is_serializable<T, BinaryOutputArchive>() && traits::has_non_member_split<T, BinaryOutputArchive>(),
                 BinaryOutputArchive &>::type
                   operator & (T const & t)
                   { 
                     //save(*this, t, traits::version<T>::value);
                     return *this;
                   }

      template<class T>
        typename std::enable_if<!traits::is_serializable<T, BinaryOutputArchive>(), BinaryOutputArchive &>::type
        operator & (T const & t)
        { 
          static_assert(traits::is_serializable<T, BinaryOutputArchive>(), "Trying to serialize an unserializable type.\n\n"
              "Types must either have a serialize function, or separate save/load functions (but not both).\n"
              "Serialize functions generally have the following signature:\n\n"
              "template<class Archive>\n"
              "  void serialize(int & ar, unsigned int version)\n"
              "  {\n"
              "    ar & member1 & member2 & member3;\n"
              "  }\n\n" );
          return *this;
        }
  };

}
