cereal - A C++11 library for serialization
==========================================

cereal is a header-only C++11 serialization library inspired by [boost](http://www.boost.org/doc/libs/1_53_0/libs/serialization/doc/index.html).  cereal takes arbitrary data types and reversibly turns them into different representations, such as compact binary encodings, XML, or JSON.  cereal was designed to be fast, light-weight, and easy to extend - it has no external dependencies and can be easily bundled with other code or used standalone.

### cereal comes with full support for C++11

Serialization support for pretty much every type in the [standard library](http://en.cppreference.com/w/) comes out of the box with cereal.  cereal also fully supports inheritance and polymorphism.  Since cereal was written to be a minimal, fast library, it does not perform the same level of object tracking as boost.  As a consequence of this raw pointers are not supported, however smart pointers (things like `std::shared_ptr` and `std::unique_ptr`) are no problem.

### cereal requires a compliant C++11 compiler

cereal uses features new to C++11 and requires a fairly compliant C++ compiler to work properly.  cereal has been confirmed to work on g++ 4.8.1 and clang++ 3.3.  It may work on older versions, but there is no emphasis on supporting them.  Until VC++ gets more complete C++11 support, it is not supported.

### cereal is fast and compact

In simple performance tests, cereal is usually faster than boost's serialization library and produces binary representations that take up less space.  When compared against another new C++11 serialization library, [Srl](https://github.com/night-shift/Srl), cereal can produce binary representations at 50% the size at 20x speed.  The source code for cereal is considerably easier to understand and extend than that of boost.

### cereal is extensible

cereal comes with binary, XML, and JSON serializers.  If you need something else, cereal was written to be easily extensible for adding custom serialization archives.

### cereal is unit tested

Trust something other than good faith - we've written a basic set of unit tests to make sure cereal is doing what it should be doing.  Somewhat ironically for a library designed to be used in place of boost, these unit tests currently require the [boost unit test framework](http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf.html) to compile.

### cereal offers a familiar syntax to users of boost

cereal's syntax will look familiar if you've used boost's serialization library.  cereal looks for serialization functions either defined in the type to be serialized or for non-member functions to do the same thing.  Unlike boost, cereal doesn't need to be told (in most cases) what type of functions to look for, and will warn you at compile time if you make a mistake.

```cpp
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
```    

### cereal has a permissive license

cereal is licensed under the [BSD license](http://opensource.org/licenses/BSD-3-Clause).

