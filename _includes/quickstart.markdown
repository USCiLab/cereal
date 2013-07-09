Quick Start
===========

This is a quick guide to get cereal up and running in a matter of minutes.  The only prerequisite for running cereal is
a modern C++11 compliant compiler, such as GCC 4.8.1 or clang 3.3.  Older versions might work, but we can't guarantee
it.

---

## Get CEREAL

cereal can be directly included in your project or installed anywhere you can access header files.  Grab the latest
version from [Github](https://github.com/USCiLab/cereal) and drop it somewhere your project can find.

---

## Add serialization methods for your classes

cereal needs to know which data members to serialize in your classes.  Let it know by implementing a `serialize` method
in your class:

```{cpp}
struct MyClass
{
  int x, y, z;

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( x, y, z ); // serialize things by passing them to the archive
  }
};
```

cereal also offers more flexible ways of writing serialization functions such as moving them outside of class
definitions or splitting them into separate load and save functions.  You can read all about that in the [serialization
functions](serialization_functions.html) section of the documentation.  cereal can also support private serialization
methods and even classes that don't support default construction.

You can serialize primitive data types and nearly every type in the [standard library](stl_support.html) without needing
to write anything yourself.

---

## Choose an archive

cereal currently supports three archive types: 
[binary] (serialization_archives.html#binary_archive), 
[XML] (serialization_archives.html#xml_archive), and 
[JSON] (serialization_archives.html#json_archive).  These archives are the middlemen between your code and your
serialized data - they handle the reading and writing for you.  XML and JSON archives are human readable but lack the
performance (both space and time) of the binary archive.  You can read all about these archives in the [archives
section](serialization_archives.html)
of the documentation.

Include your preferred archive with one of:

* `#include <cereal/archives/binary.hpp>`
* `#include <cereal/archives/xml.hpp>`
* `#include <cereal/archives/json.hpp>`

---

## Serialize your data

Create a cereal archive and send the data you want to serialize to it.  Archives are designed to be used in an [RAII](http://en.wikipedia.org/wiki/RAII)
manner and are guaranteed to flush their contents only on destruction (though it may happen earlier):

```{cpp}
#include <cereal/archives/binary.hpp>
#include <sstream>

int main()
{
  std::stringstream ss; // any stream can be used

  {
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

    MyData m1, m2, m3;
    oarchive(m1, m2, m3); // Write the data to the archive
  }

  {
    cereal::BinaryInputArchive iarchive(ss); // Create an input archive

    MyData m1, m2, m3;
    iarchive(m1, m2, m3); // Read the data from the archive
  }
}
```

### Naming values

cereal also supports name-value pairs, which lets you attach names to the objects it serializes.  This is only truly
useful if you choose to use a human readable archive format such as XML or JSON:

```{cpp}
#include <cereal/archives/xml.hpp>
#include <fstream>

int main()
{
  {
    std::ofstream os("data.xml");
    cereal::XMLOutputArchive archive(os);

    MyData m1;
    int someInt;
    double d;

    archive( CEREAL_NVP(m1), // Names the output the same as the variable name
             someInt,        // No NVP - cereal will automatically generate a delimted name
             cereal::make_nvp("this name is way better", d) ); // specify a name of your choosing
  }

  {
    std::ifstream is("data.xml");
    cereal::XMLInputArchive archive(is);
    
    MyData m1;
    int someInt;
    double d;

    archive( m1, someInt, d ); // NVPs not strictly necessary when loading
  }
}
```
---

## Learn more

cereal can do much more than these simple examples demonstrate.  cereal can handle smart pointers, polymorphism,
inheritance, and more.  Take a tour of its features by following the [documentation](index.html) or diving into the
[doxygen documentation]({{ site.baseurl }}/assets/doxygen/index.html).
