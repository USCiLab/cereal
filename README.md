cereal - A C++11 library for serialization
==========================================

cereal is a header-only C++11 serialization library inspired by [boost](http://www.boost.org/doc/libs/1_53_0/libs/serialization/doc/index.html).  cereal takes arbitrary data types and reversibly turns them into different representations, such as compact binary encodings.  cereal was designed to be fast, light-weight, and easy to extend.

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
