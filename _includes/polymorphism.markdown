Polymorphism
============

cereal supports serializing pointers to polymorphic base classes, and will automatically deduce the derived types at runtime.

---

### TLDR Version

If you want to serialize some data through pointers to base types:

1. Include `<cereal/types/polymorphic.hpp>`
2. Include all of the archives you want to be able to use with your class (`<cereal/archives/*.hpp`)
3. Use the `CEREAL_REGISTER_TYPE(YourClassName)` macro to register all of your derived classes

---

### Registering Polymorphic Types

When serializing a polymorphic base class pointer, cereal uses [Run-Time Type Information (RTTI)] (http://en.wikipedia.org/wiki/Run-time_type_information) to determine the true type of the object at the location stored in the pointer. This type information is then used to look up the proper serialization methods in a map which will have been initialized at pre-execution time. Setting up these function maps is done by calling one of two macros (`CEREAL_REGISTER_TYPE` or `CEREAL_REGISTER_TYPE_WITH_NAME`) for each type.  When writing the object to an archive, cereal will prefix your data with portable type information which is used to locate the proper serialization methods again when the archive is loaded.

**Important:** Before registering a type, you must be sure that every archive type that will be used has already been included.

```cpp
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

// Include the polymorphic serialization and registration mechanisms
#include <cereal/types/polymorphic.hpp>

#include <iostream>
#include <fstream>

// An abstract base class
struct BaseClass
{
  virtual void sayType() = 0;
  template<class Archive> void serialize( Archive & ar ) { }
};

// A class derived from BaseClass
struct DerivedClassOne : public BaseClass
{
  void sayType() { std::cout << "DerivedClassOne" << std::endl; }

  template<class Archive>
    void serialize( Archive & ar )
    { ar( x ); }

  int x;
};
CEREAL_REGISTER_TYPE(DerivedClassOne);

// Another class derived from BaseClass
struct EmbarrassingDerivedClass : public BaseClass
{
  void sayType() { std::cout << "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!" << std::endl; }
  float y;

  template<class Archive>
    void serialize( Archive & ar )
    { ar( y ); }
};
CEREAL_REGISTER_TYPE_WITH_NAME(EmbarrassingDerivedClass, "DerivedClassTwo");

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
In order for an archive to be used with polymorphic types, it must be registered with the `CEREAL_REGISTER_ARCHIVE` macro.

```cpp
namespace mynamespace
{
  class MyNewOutputArchive : public OutputArchive<MyNewArchive>
  { /* ... */ }; 
}
CEREAL_REGISTER_ARCHIVE(mynamespace::MyNewOutputArchive);
```

