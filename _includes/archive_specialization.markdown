Archive Specialization
============

cereal supports having specific serialization behaviors for different archive types.

---

### TLDR Version

You can specialize your own types or types that cereal supports by default to exhibit specific behaviors with
specific archives.  This is typically done by replacing the generic templated archive parameter (`template <class
Archive>`) with the specific
archive you wish to specialize for.  Specializations will be given precedence over any templated
serialization function.

If you want to specialize for several types of archives, you will need to use templates and type traits to restrict
template instantiation.  cereal provides several facilities to help with this, such as:
`cereal::traits::is_same_archive`, `cereal::traits::EnableIf`, and `cereal::traits::DisableIf`.

---

## Specializing Built-in Types

Although cereal comes with a support for nearly every type in the [standard library](stl_support.html), there can be
times when it is desirable to have custom functionality for a type that cereal provides the serialization for.  Function
overloading can be used to override the cereal implementation with a custom one.

### Specializing the archive

If you want to make an archive behave differently for some type, you can do this by creating overloads where the archive
is explicitly defined (and not a template parameter).

```cpp
namespace cereal
{
  // Overload the std::complex serialization code for a specific archive
  template <class T>
  void save( cereal::XMLOutputArchive & ar, std::complex<T> const & comp )
  { /* your code here */ }

  template <class T>
  void load( cereal::XMLInputArchive & ar, std::complex<T> & comp )
  { /* your code here */ }

  // Note that it doesn't make much sense to overload a plain serialize function
  // (one that does both the loading and saving) because it expects to be called
  // for loading (an input archive) and saving (an output archive).
  // We would end up needing to implement two versions of it, so using a load/save
  // pair makes more sense.
}
```

<span class="label label-warning">Important!</span>
When overloading a cereal provided serialization function, you should place it in the `xCEREAL` namespace.

### Specializing the type


The following example shows how to specialize serialization for `std::map<std::string, std::string>` such that it
roughly matches the output of an SQL \`WHERE\` clause in which all the expressions (from a name-value-pair perspective)
are ANDed together:

#### The serialization code:
```cpp
namespace cereal
{
  //! Saving for std::map<std::string, std::string> for text based archives
  // Note that this shows off some internal cereal traits such as EnableIf,
  // which will only allow this template to be instantiated if its predicates
  // are true
  template <class Archive, class C, class A,
            traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
  void save( Archive & ar, std::map<std::string, std::string, C, A> const & map )
  {
    for( const auto & i : map )
      ar( cereal::make_nvp<Archive>( i.first, i.second ) );
  }

  //! Loading for std::map<std::string, std::string> for text based archives
  template <class Archive, class C, class A> inline
  void load( Archive & ar, std::map<std::string, std::string, C, A> & map )
  {
    map.clear();

    auto hint = map.begin();
    while( true )
    {
      const auto namePtr = ar.getNodeName();

      if( !namePtr )
        break;

      std::string key = namePtr;
      std::string value; ar( value );
      hint = map.emplace_hint( hint, std::move( key ), std::move( value ) );
    }
  }
} // namespace cereal
```
<span class="label label-warning">Important!</span>
Please note that for all specializations, you must have at least one parameter that has higher precedence than the
default templated version that cereal provides.  This usually means you need to explicitly specialize at least one
template parameter.

#### Using the serialization code:
```cpp
int main()
{
  std::stringstream ss;
  {
    cereal::JSONOutputArchive ar(ss);
    std::map<std::string, std::string> filter = {% raw %}{{"type", "sensor"}, {"status", "critical"}}{% endraw %};

    ar( CEREAL_NVP(filter) );
  }

  std::cout << ss.str() << std::endl;

  {
    cereal::JSONInputArchive ar(ss);
    cereal::JSONOutputArchive ar2(std::cout);

    std::map<std::string, std::string> filter;

    ar( CEREAL_NVP(filter) );
    ar2( CEREAL_NVP(filter) );
  }

  std::cout << std::endl;
  return 0;
}
```

#### Output using xCEREAL built in support:
```json
{
  "filter": [
    { "key": "type", "value": "sensor" },
    { "key": "status", "value": "critical" }
  ]
}
```

#### Output using the above specialized serializaton code:
```json
{
    "filter": {
        "status": "critical",
        "type": "sensor"
    }
}
```



---

## Specializing User Defined Types

When serializing a type


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
In order for an archive to be used with polymorphic types, it must be registered with the `CEREAL_REGISTER_ARCHIVE` macro.  This is only important if you design a custom archive and wish for it to support polymorphism.  This is already done for all archives that come with cereal.

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
