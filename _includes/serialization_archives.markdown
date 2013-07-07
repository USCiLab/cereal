Serialization Archives
======================

cereal comes with support for binary, XML, and JSON serialization.

---

### TLDR Version

cereal comes with binary, XML, and JSON archives that allow loading and saving to these data types.  Archives can be used by including `<cereal/archives/xxxx.hpp>` for the particular archive type.  Archives need to be included before registering types for [polymorphism](polymorphism.html).

---

## Archive Basics

Archives decide how to output or interpret data that is being serialized.  For the most part, users do not need to know the inner workings of an archive and can write serialization functions agnostic of the archive type, though cereal does support specializing serialization functions for specific types of archives.  For example, a serialization function for a JSON archive could include more human readable metadata than its binary variant.

Most cereal archives operate on either an [`std::ostream`](http://en.cppreference.com/w/cpp/io/basic_ostream) or [`std::istream`](http://en.cppreference.com/w/cpp/io/basic_istream) object.  These can be files, in-memory streams, or even things like standard in and out (though the latter two may not make much sense).

The preferred way for working with archives is to create them, use (serialize with) them, and then destroy them.  Some archives (e.g. XML) are designed to output to their stream only on their destruction.  The best way to go about this is to let scoping automatically destroy archive objects when you are finished with them:

```cpp
#include <cereal/archives/xml.hpp>
#include <fstream>

{
  std::ofstream file( "out.xml" );
  cereal::XMLOutputArchive archive( os ); // depending on the archive type, data may be
                                          // output to the stream as it is serialized, or
                                          // only on destruction
  archive( some_data, more_data, data_galore );
} // when archive goes out of scope it is guaranteed to have flushed its
  // contents to its stream
```

---

### Advanced Topic: Polymorphism

Although detailed at length [elsewhere](polymorphism.html), if you will be serializing polymorphic data types through pointers to base objects, you must register the types.  This registration must happen **after** any archives are included.

---

<a name="binary_archive"></a>

## Binary Data

The binary archive can be used by including `<cereal/archives/binary.hpp>`.  The binary archive is designed to produce compact bit level representations of data and is not human readable.  It is a good choice when computers will be looking at the data on both ends of the serialization.  The binary archive is also the fastest archive that comes with cereal.  Binary archives will ignore name-value pairs and only serialize the values.

---


<a name="xml_archive"></a>

## XML

The XML archive can be used by including `<cereal/archives/xml.hpp>`.  XML is a human readable format and should not be used in situations where serialized data size is critical.  Unlike the binary archive, which outputs its data incrementally as serialization functions are called, the XML archive builds a tree in memory and only outputs it upon destruction of the archive.

XML archives, unlike binary, will utilize name-value pairs to give human readable names to its output.  If a name-value pair is not supplied, it will automatically generate a delimited name.  XML archives do not need to output size metadata for variable size containers since the number of children of a node can be queried when loading the data.  This means that it is possible to add extra data manually to an XML archive before loading it:

```cpp
#include <iostream>
#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>

int main()
{
  cereal::XMLOutputArchive archive( std::cout );
  std::vector<int> vec = {1, 2, 3, 4, 5};
  archive( CEREAL_NVP(vec) );
}
```

This causes the output XML:

```xml
<?xml version="1.0"?>
<cereal>
  <vec> <!-- Note that no size is output for the container -->
    <value0>1</value0> <!-- For things that don't have NVPs, names will be generated in a delmited fashion -->
    <value1>2</value1>
    <value2>3</value2>
    <value3>4</value3>
    <value4>5</value4>
    <!-- If desired, data could be inserted here before loading the XML -->
  </vec>
</cereal>
```

Note that if you choose to edit the generated XML by hand, you still need to make sure that the inserted data is valid.  Inserting data where there shouldn't be data will cause errors when the XML is loaded.

XML can optionally output complete demangled type information as an attribute and offer control over the precision of output floating point numbers.

---

<a name="json_archive"></a>

## JSON

The JSON archive can be used by including `<cereal/archives/json.hpp>`.  JSON is a human readable format and should not be used in situations where serialized data size is critical.

---

## Adding More Archives

Adding more archives to cereal is a fairly simple process that requires understanding a few key functions and some base types.

---

### Prologues, Epilogues, and Serialization Functions

 Special functions are called immediately before and immediately after every type is serialized, called `prologue` and `epilogue`.  The default cases for these functions looks like (found in `<cereal/cereal.hpp>`):

```cpp
template <class Archive, class T>
void prologue( Archive &, T const & )
{ }
// similarly for epilogue
```

These functions are given a reference to the active archive and a constant reference to the type being serialized.  Careful specialization of `prologue` and `epilogue` allows for archives to exercise special behaviors without having to re-implement serialization functions for types.  This lets us make the type support fairly generic.  For example, `cereal::XMLOutputArchive` (`<cereal/archives/xml.hpp>`) makes use of these functions to start and finish nodes in its in-memory tree.

Although most types are handled in a generic fashion, it is necessary to explicitly state how an archive will handle arithmetic data, since most types will be collapsed to arithmetic data eventually.  This is done in the same fashion as any other [serialization function](serialization_functions.html).  The types that must be specialized for an archive are:

* arithmetic values, detected with `std::is_arithmetic`
* name-value pairs, which use the class `cereal::NameValuePair<T>`

Optionally, an archive can also choose to support:

* C style arrays, detected with `std::is_array`
* Binary data output, detected with `cereal::BinaryData<T>`

Adding support for `cereal::BinaryData<T>` means that your archive can support optimized serialization of raw binary data.  If your archive accepts `cereal::BinaryData<T>`, cereal will attempt to package up any type (e.g. arrays of arithmetic data, strings, etc) that can be represented in this way as binary data.  If you are working with human readable output, this may not be the behavior you desire - in such cases it is best to **not** specialize for `cereal::BinaryData<T>` and instead provide users with explicit binary save/load functions that can be invoked directly on the archive, if they desire.  See `<cereal/archives/xml.hpp>` for an example of this.

---

### The Base Archives

All archives need to derive from `cereal::InputArchive` or `cereal::OutputArchive`, defined in `<cereal/cereal.hpp>`.

---

### Registering Your Archive

To enable polymorphic support, your archive must be registered with the `CEREAL_REGISTER_ARCHIVE` macro, defined in `<cereal/cereal.hpp>`.  This is best done immediately following the definition of your archive.

