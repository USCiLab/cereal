cereal - A C++11 library for serialization
==========================================

cereal is a header-only C++11 serialization library inspired by [boost](http://www.boost.org/doc/libs/1_53_0/libs/serialization/doc/index.html).  cereal takes arbitrary data types and reversibly turns them into different representations, such as compact binary encodings.  cereal was designed to be fast, light-weight, and easy to extend - it has no external dependencies and can be easily bundled with other code or used standalone.

## cereal comes with full support for the C++11 standard library

Serialization support for pretty much every type in the [standard library](http://en.cppreference.com/w/) comes out of the box with cereal.  Since cereal was written to be a minimal, fast library, it does not support serializing raw pointers - smart pointers (things like `std::shared_ptr` and `std::unique_ptr`) are no problem, however.

### cereal requires a compliant C++11 compiler

cereal uses features new to C++11 and requires a fairly compliant C++ compiler to work properly.  cereal has been confirmed to work on g++ 4.8.1 and clang++ 3.3.  It may work on older versions, but there is no emphasis on supporting them.  Until VC++ gets more complete C++11 support, it is not supported.

### cereal offers a familiar syntax to users of boost

    #include <cereal/archives/binary.hpp>
    #include <cereal/types/map.hpp>
    #include <cereal/types/memory.hpp>
    
    struct MyRecord
    {
      uint8_t x, y;
      float z;
      
      template <class Archive>
      void serialize( Archive & ar )
      {
        ar( x, y, z );
      }
    };
    
    struct SomeData
    {
      int32_t id;
      std::shared_ptr<std::unordered_map<uint32_t, MyRecord>> data;
      
      template <class Archive>
      void save( Archive & ar )
      {
        ar( data );
      }
      
      template <class Archive>
      void load( Archive & ar )
      {
        static int32_t idGen = 0;
        id = idGen++;
        ar( data );
      }
    };
