Transitioning From Boost
===========

If you are coming from the world of Boost serialization, you will find that cereal supports almost every feature you are
used to using.  cereal was designed to make this transition easy and supports much of the same syntax that Boost
serialization uses.  This is a short guide on making the transition.  Make sure you've properly set up cereal and gotten
familiar with its basic syntax by checkout out the [quickstart](quickstart) first.

---

## Key differences from Boost

cereal attempts to store as little metadata as possible when saving data.  Boost, by default, stores various metadata
about the library version as well as the types themselves.  cereal will not perform any checks to ensure that
serialization was done with the same version of cereal.

cereal is header only, meaning that there is nothing to link against.  One of the most frustrating issues when using
Boost serialization is keeping Boost versions consistent across different machines - cereal can easily be installed
standalone or embedded in your project to keep versions consistent.

cereal supports almost everything in the standard library out of the box.  Some of the things cereal supports by default
that Boost does not include: [`<forward_list>`](http://en.cppreference.com/w/cpp/container/forward_list), [`<memory>`](http://en.cppreference.com/w/cpp/header/memory), [`<queue>`](http://en.cppreference.com/w/cpp/header/queue), [`<stack>`](http://en.cppreference.com/w/cpp/header/stack), [`<tuple>`](http://en.cppreference.com/w/cpp/header/tuple), [`<unordered_set>`](http://en.cppreference.com/w/cpp/header/unordered_set), and [`<unordered_map>`](http://en.cppreference.com/w/cpp/header/unordered_map).

cereal cannot handle raw pointers and requires a fairly complete c++11 implementation to run.  From a development
standpoint, the code of cereal should be significantly easier to comprehend and extend.

cereal can be less verbose than Boost, e.g. automatically inferring the use of a split load/save pair instead of a
single serialize function without the need to use a macro.  cereal uses `static_assert` to give meaningful errors when
you make mistakes.

---

## Add serialization methods for your classes

cereal needs to know which data members to serialize in your classes.  Let it know by implementing a `serialize` method
in your class:

```{cpp}
struct MyClass
{
  int x, y, z;

  // This method lets xCEREAL know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( x, y, z ); // serialize things by passing them to the archive
  }
};
```

cereal also offers more flexible ways of writing serialization functions such as moving them outside of class
definitions or splitting them into separate load and save functions.  You can read all about that in the [serialization
functions](serialization_functions.html) section of the documentation.  cereal can also support class versioning, private serialization
methods, and even classes that don't support default construction.

You can serialize primitive data types and nearly every type in the [standard library](stl_support.html) without needing
to write anything yourself.

---

## Choose an archive

cereal currently supports three basic archive types: 
[binary] (serialization_archives.html#binary_archive) (also available in a [portable]
(serialization_archives.html#portable_binary_archive) version), 
[XML] (serialization_archives.html#xml_archive), and 
[JSON] (serialization_archives.html#json_archive).  These archives are the middlemen between your code and your
serialized data - they handle the reading and writing for you.  XML and JSON archives are human readable but lack the
performance (both space and time) of the binary archive.  You can read all about these archives in the [archives
section](serialization_archives.html)
of the documentation.

Include your preferred archive with one of:

* `#include <cereal/archives/binary.hpp>`
* `#include <cereal/archives/portable_binary.hpp>`
* `#include <cereal/archives/xml.hpp>`
* `#include <cereal/archives/json.hpp>`

---

## Serialize your data

Create a cereal archive and send the data you want to serialize to it.  Archives are designed to be used in an [RAII](http://en.wikipedia.org/wiki/RAII)
manner and are guaranteed to flush their contents only on destruction (though it may happen earlier).  Archives
generally take either an [`std::istream`](http://en.cppreference.com/w/cpp/io/basic_istream) or an [`std::ostream`](http://en.cppreference.com/w/cpp/io/basic_ostream) object in their constructor:

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
             someInt,        // No NVP - xCEREAL will automatically generate an enumerated name
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
