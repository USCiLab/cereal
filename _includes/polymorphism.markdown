Polymorphism
============

cereal supports serializing smart pointers to polymorphic base classes and will automatically deduce the derived types at runtime.

---

### TLDR Version

If you want to serialize some data through pointers to base types:

1. Include `<cereal/types/polymorphic.hpp>` (automatically included with `<cereal/types/memory.hpp>`)
2. Include all of the archives you want to be able to use with your class (`<cereal/archives/*.hpp`)
3. Read the documentation below to understand your decision for step 4 (no TLDR for this one!)
4. Use the `CEREAL_REGISTER_TYPE(YourClassName)` macro in either the header file in which the type is declared or the source
   file in which it is defined
5. You may also need to use `CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, DerivedClass)`, detailed below.

---

## Registering Polymorphic Types

When serializing a polymorphic base class pointer, cereal uses [Run-Time Type Information (RTTI)](http://en.wikipedia.org/wiki/Run-time_type_information) to determine the true type of the object at the location stored in the pointer. This type information is then used to look up the proper serialization methods in a map which will have been initialized at pre-execution time. Setting up these function maps is done by calling one of two macros (`CEREAL_REGISTER_TYPE` or `CEREAL_REGISTER_TYPE_WITH_NAME`) for each derived type. Doxygen documentation for these macros can be found [here]({{ site.baseurl }}/assets/doxygen/polymorphic_8hpp.html).

While it is not necessary to register base classes, cereal must have a serialization path from derived to base type.
Normally this is handled automatically if you serialize a base type with either `cereal::base_type` or
`cereal::virtual_base_type`. In situations where neither of these is performed, cereal will need to be explicitly told
about the relationship between base and derived type, using the `CEREAL_REGISTER_POLYMORPHIC_RELATION` macro, detailed
[here]({{ site.baseurl }}/assets/doxygen/polymorphic_8hpp.html).

When writing your polymorphic object to an archive, cereal will prefix your data with portable type information which is used to locate the proper serialization methods again when the archive is loaded.

Registration can be done from either a header file or from a source file, though there are important caveats for each,
as detailed below.

<span class="label label-warning">Important!</span>
Regardless of where registration is done, registration will only map your registered type to archives (that
themselves have been registered, see [below](#register_archive)) included prior to `CEREAL_REGISTER_TYPE` being called.  Archives that are included after type registration occurs will not bind with the type, leading to runtime errors when serialized.

### Registering from a header file

Registration can be done safely in a header file so long as the appropriate archives are included prior to registration.
Registration will always occur in the correct translation unit so long as the header file that contains the registration
is included.

The potential downside to performing registration in a header file are that you may force the inclusion of additional 
cereal header files, especially archives, on anyone that includes your header.  In cases where you want to hide implementation
details or avoid bringing in additional includes, you may want to consider performing source file registration.  The
upside to header registration is that it is very simple, and so long as the correct archives are included, should work
under any situation.

#### Header registration example

##### myclasses.hpp
```cpp
// Include the polymorphic serialization and registration mechanisms
#include <cereal/types/polymorphic.hpp>

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

// Include any archives you plan on using with your type before you register it
// Note that this could be done in any other location so long as it was prior
// to this file being included
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

// Register DerivedClassOne
CEREAL_REGISTER_TYPE(DerivedClassOne);

// Register EmbarassingDerivedClass with a less embarrasing name
CEREAL_REGISTER_TYPE_WITH_NAME(EmbarrassingDerivedClass, "DerivedClassTwo");

// Note that there is no need to register the base class, only derived classes
//  However, since we did not use cereal::base_class, we need to clarify
//  the relationship (more on this later)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, DerivedClassOne)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, EmbarrassingDerivedClass)
```

##### myclasses.cpp
```cpp
#include "myclasses.hpp"
#include <iostream>

void DerivedClassOne::sayType()
{
  std::cout << "DerivedClassOne" << std::endl; 
}

void EmbarrassingDerivedClass::sayType()
{
  std::cout << "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!" << std::endl; 
}

```

##### main.cpp
```cpp
// any archives included prior to 'myclasses.hpp' 
// would also apply to the registration
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

### Registering from a source file

Registration can also be done within a source file.  The same expectation of including appropriate archives beforehand
still applies.  However, since this is occuring in a source file, the inclusion of additional cereal headers can be
hidden as an implementation detail and not leak to others that include the header declaring your types.  In addition,
type registration can safely be called repeatedly in different translation units with different archives, properly
binding each additional archive to the type as it becomes available.  In other words, you can re-register the same type
in different source files with different archives included beforehand, and serialize to any of those archives safely.

Be careful that there are special considerations to make for placing registration in a source file, especially if you
will not be explicitly referencing anything within that source file.  See the [documentation]({{ site.baseurl }}/assets/doxygen/polymorphic_8hpp.html#a0479821f6a987e08e7d3f765cda02d91) for the macro
`CEREAL_REGISTER_DYNAMIC_INIT` and `CEREAL_FORCE_DYNAMIC_INIT` for more information.  This is especially important to
read if you are creating shared or static libraries for your serialization code.

<span class="label label-warning">Important!</span>
If you are building a dynamic library on Windows, registration must occur within the header file.  Source file
registration will not properly occur outside of the DLL.  This issue is not present on Linux or Mac.

### Registration Example

##### myclasses.hpp
```cpp
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

// Potentially necessary if no explicit reference
// to objects in myclasses.cpp will take place
// from other translation units
//#include <cereal/types.polymorphic.hpp>
//CEREAL_FORCE_DYNAMIC_INIT(myclasses)
```

##### myclasses.cpp

```cpp
#include "myclasses.hpp"
#include <iostream>

// Include any archives you plan on using with your type before you register it.
// This could occur in multiple source files for the same type, if desired.
// Any archives included from other header files included here will also be
// registered.
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
//  However, since we did not use cereal::base_class, we need to clarify
//  the relationship (more on this later)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, DerivedClassOne)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, EmbarrassingDerivedClass)

// Potentially necessary if no explicit reference
// to objects in myclasses.cpp will take place
// from other translation units
//CEREAL_REGISTER_DYNAMIC_INIT(myclasses)
```

##### main.cpp
```cpp
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

### Ensuring a path exists from derived to base type

As mentioned earlier, cereal does not require that the base type itself is registered; only derived classes that are
used need to be registered. However, cereal still needs to know how to properly convert between derived and base types.
Normally, cereal can determine this automatically if your derived class serializes the base class:

```cpp
struct Base
{
  virtual void foo() = 0;
  int x;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x );
  }
};

struct DerivedOne: Base
{
  void foo() {}

  double y;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( cereal::base_class<Base>(this), y );
  }
};

CEREAL_REGISTER_TYPE(DerivedOne)
```

With the above example, the `base_class` usage lets cereal automatically deduce that `DerivedOne` is related to `Base`.
Cereal requires that all possible base classes that are actually used to store a pointer have a well defined path from
the derived class. If cereal cannot detect this automatically via `base_class` serializations, or if you have a
situation where there is no reason to serialize the base class (e.g. it is pure virtual), you must use a macro to
disambiguate for cereal:

```cpp
struct EmptyBase
{
  virtual void foo() = 0;
};

struct DerivedTwo: EmptyBase
{
  void foo() {}

  double y;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( y );
  }
};

CEREAL_REGISTER_TYPE(DerivedTwo)
CEREAL_REGISTER_POLYMORPHIC_RELATION(EmptyBase, DerivedTwo)
```

See the [doxygen]({{ site.baseurl }}/assets/doxygen/polymorphic_8hpp.html) for more information.

---

<a name="register_archive"></a>

## Registering Archives

In order for an archive to be used with polymorphic types, it must be registered with the `CEREAL_REGISTER_ARCHIVE` macro.  This is only important if you design a custom archive and wish for it to support polymorphism.  This is already done for all archives that come with cereal.  You can read more about this macro in the [doxygen documentation]({{ site.baseurl }}/assets/doxygen/group__Internal.html#ga80fe42796f7a4d6132ade9cb632cb1d1).

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

## Implementation notes

The implementation for polymorphic support can be considered a simplified version of that found in boost's serialization library.  Please
see `<cereal/types/polymorphic.hpp>` and `<cereal/details/polymorphic_impl.hpp>` for acknowledgement and implementation
notes.
