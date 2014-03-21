Polymorphism
============

cereal supports serializing smart pointers to polymorphic base classes and will automatically deduce the derived types at runtime.

---

### TLDR Version

If you want to serialize some data through pointers to base types:

1. Include `<cereal/types/polymorphic.hpp>`
2. Include all of the archives you want to be able to use with your class (`<cereal/archives/*.hpp`)
3. Use the `CEREAL_REGISTER_TYPE(YourClassName)` macro in a .cpp file to register all of your derived classes

---

### Registering Polymorphic Types

When serializing a polymorphic base class pointer, cereal uses [Run-Time Type Information (RTTI)] (http://en.wikipedia.org/wiki/Run-time_type_information) to determine the true type of the object at the location stored in the pointer. This type information is then used to look up the proper serialization methods in a map which will have been initialized at pre-execution time. Setting up these function maps is done by calling one of two macros (`CEREAL_REGISTER_TYPE` or `CEREAL_REGISTER_TYPE_WITH_NAME`) for each derived type.  When writing the object to an archive, cereal will prefix your data with portable type information which is used to locate the proper serialization methods again when the archive is loaded.

<span class="label label-warning">Important!</span>
Before registering a type, you must be sure that every archive type that will be used has already been included.

<span class="label label-warning">Important!</span>
Calling `CEREAL_REGISTER_TYPE` in a header file will result in linker errors if the header is included more than once. Call it in a .cpp file instead.

#### myclasses.hpp
``` {cpp}
 // A pure virtual base class
struct BaseClass
{
  virtual void sayType() = 0;
};

// A class derived from BaseClass
struct DerivedClassOne : public BaseClass
{
  void sayType();

  int x;

  template<class Archive>
    void serialize( Archive & ar )
    { ar( x ); }
};

// Another class derived from BaseClass
struct EmbarrassingDerivedClass : public BaseClass
{
  void sayType();

  float y;

  template<class Archive>
    void serialize( Archive & ar )
    { ar( y ); }
};
```

#### myclasses.cpp
``` {cpp}

#include "myclasses.hpp"
#include <iostream>

// Include any archives you plan on using with your type before you register it
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

// Include the polymorphic serialization and registration mechanisms
#include <cereal/types/polymorphic.hpp>

void DerivedClassOne::sayType()
{
  std::cout << "DerivedClassOne" << std::endl; 
}

void EmbarrassingDerivedClass::sayType()
{
  std::cout << "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!" << std::endl; 
}

// Register DerivedClassOne
CEREAL_REGISTER_TYPE(DerivedClassOne);

// Register EmbarassingDerivedClass with a less embarrasing name
CEREAL_REGISTER_TYPE_WITH_NAME(EmbarrassingDerivedClass, "DerivedClassTwo");

// Note that there is no need to register the base class, only derived classes

```

#### main.cpp
``` {cpp}
#include "myclasses.hpp"

#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>

#include <iostream>
#include <fstream>

int main()
{
  {
    std::ofstream os( "polymorphism_test.xml" );
    cereal::XMLOutputArchive oarchive( os );

    // Create instances of the derived classes, but only keep base class pointers
    std::shared_ptr<BaseClass> ptr1 = std::make_shared<DerivedClassOne>();
    std::shared_ptr<BaseClass> ptr2 = std::make_shared<EmbarrassingDerivedClass>();
    oarchive( ptr1, ptr2 );
  }

  {
    std::ifstream is( "polymorphism_test.xml" );
    cereal::XMLInputArchive iarchive( is );

    // De-serialize the data as base class pointers, and watch as they are
    // re-instantiated as derived classes
    std::shared_ptr<BaseClass> ptr1;
    std::shared_ptr<BaseClass> ptr2;
    iarchive( ptr1, ptr2 );

    // Ta-da! This should output:
    ptr1->sayType();  // "DerivedClassOne"
    ptr2->sayType();  // "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!"
  }

  return 0;
}
```

---

### Registering Archives
In order for an archive to be used with polymorphic types, it must be registered with the `CEREAL_REGISTER_ARCHIVE` macro.  This is only important if you design a custom archive and wish for it to support polymorphism.  THis is already done for all archives that come with cereal.

```cpp
namespace mynamespace
{
  class MyNewOutputArchive : public OutputArchive<MyNewArchive>
  { /* ... */ }; 
}
CEREAL_REGISTER_ARCHIVE(mynamespace::MyNewOutputArchive)
```

It is recommended to do this immediately following the declaration of your archive.

---

### Implementation notes

The implementation for polymorphic support can be considered a simplified version of that found in boost's serialization library.  Please
see `<cereal/types/polymorphic.hpp>` and `<cereal/details/polymorphic_impl.hpp>` for acknowledgement and implementation
notes.
