Serialization Archives
======================

cereal comes with support for binary, XML, and JSON serialization.

---

### TLDR Version

cereal comes with binary, XML, and JSON archives that allow loading and saving to these data types.  Archives can be used by including `<cereal/archives/xxxx.hpp>` for the particular archive type.  Archives need to be included before registering types for [polymorphism](polymorphism.html).

---

## Archive Basics

Archives decide how to output or interpret data that is being serialized.  For the most part, you do not need to know the inner workings of an archive and can write serialization functions agnostic of the archive type, though cereal does support specializing serialization functions for specific types of archives (e.g. a serialization function for a JSON archive could include more human readable metadata than its binary variant).

cereal archives operate on either an [`std::ostream`](http://en.cppreference.com/w/cpp/io/basic_ostream) or [`std::istream`](http://en.cppreference.com/w/cpp/io/basic_istream) object.  These can be files, in-memory streams, or even things like standard in and out (though the latter two may not make much sense).

The preferred way for working with archives is in an [RAII](http://en.wikipedia.org/wiki/RAII) fashion.  Archives are only guaranteed to have flushed their contents when they are destroyed, so some archives (e.g. XML) will not output anything until their destruction.  The best way to go about using archives is to let scoping automatically destroy archive objects when you are finished with them:

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

cereal was not designed to be a robust long term storage solution - it is your responsibility to ensure compatability
between saved and loaded cereal archives.  It is recommended that you use the same version of cereal for both loading
and saving data.  

cereal does support including version information along with the data it serializes in a fashion similar to that found
in boost.  For more information on this see the [versioning](serialization_functions.html#versioning) section of the serialization types documentation.

### Advanced topic: polymorphism

Although detailed at length [elsewhere](polymorphism.html), if you will be serializing polymorphic data types through pointers to base objects, you must register the types.  This registration must happen **after** any archives are included.

---

<a name="binary_archive"></a>

## Binary Data

The binary archive can be used by including `<cereal/archives/binary.hpp>`.  The binary archive is designed to produce compact bit level representations of data and is not human readable.  It is a good choice when computers will be looking at the data on both ends of the serialization.  The binary archive is also the fastest archive that comes with cereal.  Binary archives will ignore name-value pairs and only serialize the values.  

The binary archive makes no attempt to ensure that endianness is preserved across different architectures.  If your data will be read on both little and big-endian machines, you should use `<cereal/archives/portable_binary.hpp>`, which tracks the endianness of the saving and loading machines and transforms data appropriately.  It has slightly more overhead than the regular binary archive.

When using a binary archive and a file stream (`std::fstream`), remember to specify the binary flag (`std::ios::binary`)
when constructing the stream.  This prevents the stream from interpreting your data as ASCII characters and altering
them.

---

<a name="xml_archive"></a>

## XML

The XML archive can be used by including `<cereal/archives/xml.hpp>`.  XML is a human readable format and should not be used in situations where serialized data size is critical.  Unlike the binary archive, which outputs its data incrementally as serialization functions are called, the XML archive builds a tree in memory and only outputs it upon destruction of the archive.

XML archives, unlike binary, will utilize name-value pairs to give human readable names to its output.  If a name-value pair is not supplied, it will automatically generate an enumerated name.  XML archives do not need to output size metadata for variable sized containers since the number of children of a node can be queried when loading the data.  This means that it is possible to add extra data manually to an XML archive before loading it:

```cpp
#include <iostream>
#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>

int main()
{
  cereal::XMLOutputArchive archive( std::cout );
  bool arr[] = {true, false};
  std::vector<int> vec = {1, 2, 3, 4, 5};
  archive( CEREAL_NVP(vec),
           arr );
}
```

This causes the output XML:

```xml
<?xml version="1.0"?>
<cereal>
  <vec size="dynamic"> <!-- Note that there is no listed size; it is marked as dynamic -->
    <value0>1</value0> <!-- Names will be automatically created if NVPs are not supplied -->
    <value1>2</value1>
    <value2>3</value2>
    <value3>4</value3>
    <value4>5</value4>
    <!-- If desired, data could be inserted here before loading the XML -->
  </vec>
  <value0> <!-- Note that since an array is fixed size, there is no size information -->
    <value0>true</value0>
    <value1>false</value1>
  </value0>
</cereal>
```

Note that if you choose to edit the generated XML by hand, you still need to make sure that the inserted data is valid.  Inserting data where there shouldn't be data will cause errors when the XML is loaded.  You can only insert data into dynamically sized containers.  

XML can optionally output complete demangled type information as an attribute and offers control over the output precision of floating point numbers.  If you need to have binary equality between floating point numbers, you will need a significant precision for the output (on the order of 10 for floats, 20 for doubles, 40 for long doubles).  cereal will use the largest number of digits appropriate for serializing a double by default.

The XML serialization is powered by [RapidXML](http://rapidxml.sourceforge.net/).

<a name="outoforder"></a>
### Out of order loading

The default behavior for all archives is to sequentially load data in the order in which it appears.
XML (and JSON) archives support out of order loading, meaning that you can utilize name-value pairs to load data in an order
different to that in which it appears in the XML file. 

When cereal detects that you are using an NVP to load data from an XML archive, it checks to see if the name matches the
next expected (in order) name.  If they don't match, cereal will search for the provided name within the current level
of nodes.  If the name isn't found an exception will be thrown.  Once cereal finds and loads the name, it will proceed
sequentially from that location until forced to search for another name via an NVP.

Consider the following XML:

```{xml}
<?xml version="1.0"?>
<cereal>
  <var1>4</var1>
  <value0>32</value0>
  <value1>64</value1>
  <myData>
    <value0>true</value0>
    <another>3.24</another>
  </myData>
  <value2>128</value2>
</cereal>
```

The following code shows how it could be loaded in an out of order fashion:

```{cpp}
#include <cereal/archives/xml.hpp>
#include <iostream>

struct MyData
{
  bool   b;
  double d;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( b, d );
  }
};

int main()
{
  int i1, i2, i3, i4;
  MyData md;

  {
    std::ifstream is("data.xml");
    cereal::XMLInputArchive ar(is);

    // NVP doesn't match expected value, perform a search
    ar( cereal::make_nvp("myData", md) );

    ar( i4 );                           // xCEREAL continues from node it found using search
    ar( cereal::make_nvp("var1", i1) ); // new search re-positions at node var1
    ar( i2, i3 );                       // continue from location of last search

                                                // next value read would be 'myData'
    ar( cereal::make_nvp("doesNotExist", i1) ); // throws an exception: NVP not found
  }

  return 0;
}

```

<span class="label label-warning">Important!</span>
cereal's default behavior is to load in the order data is presented in an archive.  If you use an NVP to load something
out of order, cereal will immediately revert to this behavior starting at the node you caused it to jump to.

<a name="binaryoutput"></a>
### Binary output

XML archives also support explicit binary input/output, which will encode the data as a [base64](http://en.wikipedia.org/wiki/Base64) string:

```{cpp}
#include <cereal/archives/xml.hpp>
#include <iostream>

int main()
{
  cereal::XMLOutputArchive archive( std::cout );

  int arr[] = {-1, 95, 3};
  archive.saveBinaryValue( xxx, sizeof(int) * 3, "some_optional_name" );
}
```

which will output:

```xml
<?xml version="1.0"?>
<cereal>
  <some_optional_name>/////18AAAADAAAA</some_optional_name>
</cereal>

```

This data can be loaded in a similar fashion with `loadBinaryValue`.

---

<a name="json_archive"></a>

## JSON

The JSON archive can be used by including `<cereal/archives/json.hpp>`.  JSON
is a human readable format and should not be used in situations where
serialized data size is critical.  JSON archives are very similar to XML
archives in that they will take advantage of name-value pairs when available,
and automatically generate names when they are not.  Consider the following
nearly identical example to that seen in the XML section:

```cpp
#include <iostream>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

int main()
{
  cereal::JSONOutputArchive archive( std::cout );
  bool arr[] = {true, false};
  std::vector<int> vec = {1, 2, 3, 4, 5};
  archive( CEREAL_NVP(vec),
           arr );
}
```

This causes the output JSON:

```json
{
    "vec": [
        1,
        2,
        3,
        4,
        5
    ],
    "value0": {
        "value0": true,
        "value1": false
    }
}
```

Note how dynamically sized containers (e.g. `std::vector`) are serialized as JSON arrays, while fixed sized containers
(e.g. `std::array`) are serialized as JSON objects.  This distinction is important as it allows you to hand insert data
into variable sized containers in a JSON file by inserting elements into an array, whereas you cannot insert new data
into objects.  You can still hand edit values in objects, but you cannot append or
deduct data from them.

<span class="label label-info">Serializing numeric types with JSON:</span>
All numeric types are serialized as numbers except for large types such as long long, unsigned long long, and long double, which are serialized as strings.  In JSON, numbers will not have surrounding quotes, while the string representations of the larger types will be surrounded with double quotes.

The JSON serialization is powered by [rapidjson](http://code.google.com/p/rapidjson/).

### Out of order loading

The JSON archive, like the XML archive, supports out of order loading.  For details on how this work, please see the
discussion [above](#outoforder) for the XML archive.

### Binary output

Just like XML, JSON archives support explicit binary output by encoding data in base64 strings.  The syntax is identical
to the [above](#binaryoutput) XML example using the functions `saveBinaryValue` and `loadBinaryValue`.

---

## Adding More Archives

Adding more archives to cereal is a fairly simple process that requires understanding a few key functions and some base types.

### Prologues, epilogues, and serialization functions

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

* Binary data output, detected with `cereal::BinaryData<T>`
* `std::string`, which by default will try to use `cereal::BinaryData<T>` encodings.
* Minimal representation output, detected with `cereal::traits::has_minimal_output_serialization<T>` (or `cereal::traits::has_minimal_input_serialization<T>` for input)

Adding support for `cereal::BinaryData<T>` means that your archive can support optimized serialization of raw binary data.  If your archive accepts `cereal::BinaryData<T>`, cereal will attempt to package up any type (e.g. arrays of arithmetic data, strings, etc) that can be represented in this way as binary data.  If you are working with human readable output, this may not be the behavior you desire - in such cases it is best to **not** specialize for `cereal::BinaryData<T>` and instead provide users with explicit binary save/load functions that can be invoked directly on the archive, if they desire.  See `<cereal/archives/xml.hpp>` for an example of this.

Note that the generic implementation for `std::string` is to serialize its data in a binary fashion by wrapping the
internals in `cereal::BinaryData<T>`.  If your archive does not support this, or you wish to have different behavior for
strings, such as outputting readable characters, you will need to specialize string serialization functions for your archive.

Minimal representation serialization, detailed ELSEWHERE, is usually only utilized by human readable archives.  This
allows you to specialize for cases when outputting extra information or extra nodes is unnecessary.  For an example of
this in action, look at how enum serialization (`<cereal/types/common.hpp>`) is performed in the JSON and XML archives.

### The base archives

All archives need to derive from `cereal::InputArchive` or `cereal::OutputArchive`, defined in `<cereal/cereal.hpp>`.

### Registering your archive

To enable polymorphic support, your archive must be registered with the `CEREAL_REGISTER_ARCHIVE` macro, defined in `<cereal/cereal.hpp>`.  This is best done immediately following the definition of your archive.

### Learn more

The best way to learn how to write archives is by digging into the [doxygen documentation]({{ site.baseurl }}/assets/doxygen/index.html) and looking at the various archives in `<cereal/archives>`.
