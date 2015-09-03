Transitioning From Boost
===========

If you are coming from the world of Boost serialization, you will find that cereal supports almost every feature you are
used to using.  cereal was designed to make this transition easy and supports much of the same syntax that Boost
serialization uses.  This is a short guide on making the transition.  Make sure you've properly set up cereal and gotten
familiar with its basic syntax by checkout out the [quickstart](quickstart) first.

---

### TLDR Version

cereal has a very similar interface to Boost serialization and in some cases can immediately be dropped in by simply
replacing the Boost archives with cereal archives.  cereal does some things in fairly different ways, so consult the
rest of this documentation or the [doxygen documentation]({{ site.baseurl }}/assets/doxygen/index.html) for more details.

---

## Key Differences From Boost

* cereal attempts to store as little metadata as possible when saving data.  Boost, by default, stores various metadata
about the library version as well as the types themselves.  cereal will not perform any checks to ensure that
serialization was done with the same version of cereal.

* cereal is header only, meaning that there is nothing to link against.  One of the most frustrating issues when using
Boost serialization is keeping Boost versions consistent across different machines - cereal can easily be installed
standalone or embedded in your project to keep versions consistent.

* cereal supports almost everything in the standard library out of the box.  Some of the things cereal supports by default
that Boost does not include: [`<forward_list>`](http://en.cppreference.com/w/cpp/container/forward_list), [`<memory>`](http://en.cppreference.com/w/cpp/header/memory), [`<queue>`](http://en.cppreference.com/w/cpp/header/queue), [`<stack>`](http://en.cppreference.com/w/cpp/header/stack), [`<tuple>`](http://en.cppreference.com/w/cpp/header/tuple), [`<unordered_set>`](http://en.cppreference.com/w/cpp/header/unordered_set), and [`<unordered_map>`](http://en.cppreference.com/w/cpp/header/unordered_map).

* cereal cannot handle raw pointers and requires a fairly complete c++11 implementation to run.  From a development
standpoint, the code of cereal should be significantly easier to comprehend and extend.

* cereal can be less verbose than Boost, e.g. automatically inferring the use of a split load/save pair instead of a
single serialize function without the need to use a macro.  cereal uses `static_assert` to give meaningful errors when
you make mistakes.

* cereal has a different preferred syntax for serialization.  While Boost uses `&`, `<<`, and `>>` to send things to
archives, the preferred style in cereal is to use the `()` operator upon an archive, e.g. `archive( myData1, myData2 )`.  To ease
the transition, cereal also supports all of the aforementioned Boost syntax.  Many other features of cereal bear strong
resemblence to Boost but may have different names or operate in a slightly different manner - it is suggested to
carefully read the doxygen before using any foreign cereal features.

---

## An Example Transition

To start using cereal, you may not even need to change any of your serialization code.  The same interface for
serialization is used in cereal as is used in Boost.  Consider this example with the minimal changes necessary to
support using cereal:

```cpp
#include <boost/archive/binary_oarchive.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

class SomeData
{
  public:
    SomeData() = default;
    int a;
    int b;

  private:
    friend class cereal::access; // befriend the xCEREAL version of access
    friend class boost::serialization::access;

    // xCEREAL supports class versioning although it is considered
    // optional in xCEREAL
    template <class Archive>
    void save( Archive & ar, std::uint32_t const version ) const
    {
      // xCEREAL supports Boost style syntax until you can change it
      ar << a << b;
    }

    template <class Archive>
    void load( Archive & ar, std::uint32_t const version )
    {
      ar >> a;
      ar >> b;
    }

  // xCEREAL will figure out you've used split member load/save automatically
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

// Note that the Boost macro BOOST_SERIALIZATION_SPLIT_MEMBER
// creates a function named serialize, which creates a conflict for 
// xCEREAL, which does not allow more than one type of serialization
// function to be present.
//
// To get around this, you should temporarily use specialization (see access.hpp)
// to prevent xCEREAL from seeing this as an error.  Once you remove the Boost
// macro, you may remove this specialization as well
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES( SomeData, cereal::specialization::member_load_save )

BOOST_CLASS_VERSION(SomeData, 1);
CEREAL_CLASS_VERSION(SomeData, 1); // many things have the same or similar names in xCEREAL

struct MyType
{
  int x;
  double y;
  SomeData s;

  template <class Archive>
  void serialize( Archive & ar, std::uint32_t const version )
  {
    ar & x & y; // the & operator is valid in xCEREAL but not the preferred interface
    ar & s;
  }
};

int main()
{
  std::ofstream os("out.bin", std::ios::binary);

  // using boost
  {
    boost::archive::binary_oarchive ar(os);
    MyType m;
    ar << m; // xCEREAL supports Boost style serialization syntax until you can fully transition
  }

  // using cereal
  {
    cereal::BinaryOutputArchive ar(os);
    MyType m;
    ar << m;
  }
  
  return 0;
}
```

The only necessary change to the above code was using a cereal archive instead of a Boost archive and adding the cereal named friend and class versioning.  If you are using other Boost features, there will be more changes to make to your code.  

In Boost, if you use a split load/save pairing, you must use the `BOOST_MEMBER_SPLIT` macro, whereas cereal has no equivalent - simply create your split load/save pair.

If you befriend `boost::serialization::access`, that must change to `cereal::access`, defined in `<cereal/access.hpp>`.

In cereal, class versioning is an exception and not the rule, so most serialization functions will ommit the `std::uint32_t
 const` secondary parameter.  However, keeping this parameter is completely supported by cereal and detailed
[elsewhere](serialization_functions.html#versioning).  The macro `BOOST_CLASS_VERSION` has an equivalent called `CEREAL_CLASS_VERSION`.  Many other features have very similar
names or similar functionality, but will not always be the same.  It is highly recommended you consult the detailed
doxygen documentation on any feature that is unknown to you.

Below is the above code re-written in the preferred cereal style:


```cpp
#include <cereal/archives/binary.hpp>
#include <fstream>

class SomeData
{
  public:
    SomeData() = default;
    int a;
    int b;

  private:
    friend class cereal::access;

    // xCEREAL supports class versioning although it is considered
    // optional in xCEREAL
    template <class Archive>
    void save( Archive & ar, std::uint32_t const version ) const
    {
      ar( a, b ); // operator() is the preferred way of interfacing the archive
    }

    template <class Archive>
    void load( Archive & ar, std::uint32_t const version )
    {
      ar( a, b );
    }

    // note the lack of explicitly informing xCEREAL to use a split member load/save
};

CEREAL_CLASS_VERSION(SomeData, 1);

struct MyType
{
  int x;
  double y;
  SomeData s;

  template <class Archive>
  void serialize( Archive & ar, std::uint32_t const version )
  {
    ar( x, y );
    ar( s );
  }
};

int main()
{
  std::ofstream os("out.bin", std::ios::binary);

  {
    cereal::BinaryOutputArchive ar(os);
    MyType m;
    ar( m );
  }
  
  return 0;
}
```
