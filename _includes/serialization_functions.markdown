Serialization Functions
=======================

Since C++ lacks 
<a href="http://en.wikipedia.org/wiki/Reflection_(computer_programming)">reflection</a>,
implementing serialization requires you to specify which data members
should be serialized. cereal provides a number of ways to specify such
serialization functions for your classes, even if you don't have access to
their internals.

---

### TLDR Version

cereal supports single serializaton functions (`serialize`) or split load/save pairs (`load` and `save`) either inside
or outside of classes.  Load and save functions can optionally be designated as minimal (`load_minimal` and
`save_minimal`).  

Internal serialization functions can be kept private so long as cereal is given access by
befriending `cereal::access`.  You can optionally choose to store versioning information by adding an additional
parameter to your serialization functions (`std::uint32_t const version`) and using the `CEREAL_CLASS_VERSION` macro.
cereal will tell you if you've made a mistake at compile time, if possible.

---

## Types of Serialization Functions

Serialization functions can either be internal or external. Functionality can
either be in a single `serialize` function, or a split `load` and `save`
functions.  Load and save functions can optionally be made to emit minimal representations,
as detailed [later](#minimal).

When possible, it is preferred to use a single internal `serialize`
method, though split methods can be used when it is necessary (e.g. to
dynamically allocate memory upon loading a class).  Unlike Boost, there is no need to explicitly tell cereal that it
needs to use the split load-save pair; cereal will pick whichever is present and give a compile time error if it cannot
disambiguate a single serialization method.

<br/>
{% capture class_begin %}struct MyClass 
{
  int x, y, z;{% endcapture %}


<div class="row">
  <div class="span1"></div>
  <div class="span5">

  Internal serialize function

  {% highlight cpp %}
{{ class_begin }}

  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( x, y, z ); 
  }
};





  {% endhighlight %}
  </div>
  <div class="span5">

  Internal split/load functions

  {% highlight cpp %}
{{ class_begin }}

  template<class Archive>
  void save(Archive & archive) const
  {
    archive( x, y, z ); 
  }

  template<class Archive>
  void load(Archive & archive)
  {
    archive( x, y, z ); 
  }
};{% endhighlight %}
  </div>
</div>

<div class="row">
  <div class="span1"></div>
  <div class="span5">

  External serialize function

  {% highlight cpp %}
{{ class_begin }} 
};

template<class Archive>
void serialize(Archive & archive,
               MyClass & m)
{
  archive( m.x, m.y, m.z );
}






  {% endhighlight %}
  </div>

  <div class="span5">

  External split/load functions

  {% highlight cpp %}
{{ class_begin }}
};

template<class Archive>
void save(Archive & archive, 
          MyClass const & m)
{ 
  archive( m.x, m.y, m.z ); 
}

template<class Archive>
void load(Archive & archive,
          MyClass & m)
{
  archive( m.x, m.y, m.z ); 
} {% endhighlight %}
  </div>
</div>
<br/>

<span class="label label-warning">Important!</span> Save functions are **const** (or take a **const** reference to your class).  cereal will throw a static assertion if it detects a non const save function.

External serialization functions should be placed either in the same namespace as the types they serialize or in the
`cereal` namespace so that the compiler can find them properly.

### Non-public serialization

Serialization functions can be placed under access control to be protected or private.  cereal will need access to them,
and can be given access by befriending `cereal::access`, defined in `<cereal/access.hpp>`:

```{cpp}
#include <cereal/access.hpp>

class MyCoolClass
{
  private:
    int secretX;

    friend class cereal::access;

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( secretX );
    }
};
```

This also works with split save/load functions.

<a name="minimal"></a>
### Minimal split serialization

In addition to supporting the normal load/save split serialization functions, cereal also supports
`minimal_load` and `minimal_save` serialization functions.  These serialization functions follow the same rules as the
others (you may only have one type of serialization per archive-type pair) but have a slightly different interface and
can change output behavior.

Minimal serialization functions are designed to take a type and reduce it to a single primitive or string value.  The
primary reason to use minimal serialization is to simplify the output to a human readable archive (e.g. XML or JSON).

<div class="row">
  <div class="span1"></div>
  <div class="span5">
  Internal split minimal serialization

{% highlight cpp %}
struct MyData
{
  double d;
  
  template <class Archive>
  double save_minimal( 
    Archive const & ) const
  {
    return d;
  }

  template <class Archive>
  void load_minimal( Archive const &,
    double const & value )
  {
    d = value;
  }


};
{% endhighlight %}
  </div>
  <div class="span5">

  External split minimal serialization

{% highlight cpp %}
struct MyData
{
  double d;
};
  
template <class Archive>
double save_minimal( 
  Archive const &,
  MyData const & md )
{
  return md.d;
}

template <class Archive>
void load_minimal( Archive const &,
  MyData & md,
  double const & value )
{
  md.d = value;
}
{% endhighlight %}
  </div>
</div>

There are a few key differences in the interface of the minimal functions:

1. They accept their `Archive` template parameter by constant reference.
2. `save_minimal` returns the minimal representation, which can either be an arithmetic type or an `std::string`.
3. `load_minimal` is passed a constant reference to the return type of `save_minimal`.

Minimal serialization can only emit a single value and are only useful when this approach makes sense.  Consider this
example showing the differences between minimal and standard serialization (serialize or split load/save pair):

```cpp
#include <cereal/archives/json.hpp>
#include <iostream>

struct Minimal
{
  std::string myData;

  template <class Archive>
  std::string save_minimal( Archive const & ) const
  { return myData; }

  template <class Archive>
  void load_minimal( Archive const &, std::string const & value )
  { myData = value; }
};

struct Normal
{
  std::string myData;

  template <class Archive>
  void serialize( Archive & ar )
  { ar( myData ); }
};

int main()
{
  Minimal m = {"minimal"};
  Normal  n = {"normal"};

  cereal::JSONOutputArchive ar( std::cout );
  ar( CEREAL_NVP(m), CEREAL_NVP(n) );
}
```

which produces the output:

```json
{
    "m": "minimal",
    "n": {
        "value0": "normal"
    }
}
```

Note the lack of an internal node for the minimal representation.

<a name="versioning"></a>
### Explicit versioning

cereal supports adding explicit versioning information for types, much like Boost class versioning.
This is optional in cereal and by default is not used for any type.  You can choose to use versioning by adding
an additional parameter to your serialization functions (regardless of which serialization style you are using), a `const std::uint32_t`, typically named `version`.

This parameter will always be given the appropriate version number by cereal.  When saving data, cereal looks for an
explicit version which you can specify with the `CEREAL_CLASS_VERSION` macro (see the [doxygen docs]({{ site.baseurl }}/assets/doxygen/group__Utility.html) for detailed information).  This macro takes a type and a version
number, and causes cereal to serialize this version information when it saves that type using a versioned serialization
function.  If you use this macro but do not use a versioned serialization function, no version information will be
saved.  If you use a versioned serialization function and do not specify the version number using the macro, cereal will
default to giving a version number of zero.

When performing loads, cereal will load versioning information if your serialization function is versioned.  If you did
not use a versioned serialization function to create the archive you are loading from, your data will be corrupted and your program will likely crash.  This
loaded version number will be supplied via the version parameter you add to your serialization functions.

<span class="label label-warning">Important!</span> If you choose to use versioning, you must ensure that all serialization functions for a type support the versioning
parameter (both your load and save must have it in the case of split serialization).

Here is a small example of adding versioning to both an internal serialize function as well as an externally split
load/save pair:

```{cpp}
#include <cereal/cereal.hpp>

struct MyCoolClass
{
  // xCEREAL will supply the version automatically when loading or saving
  // The version number comes from the CEREAL_CLASS_VERSION macro
  template <class Archive>
  void serialize( Archive & ar, std::uint32_t const version )
  {
    // You can choose different behaviors depending on the version
    // This is useful if you need to support older variants of your codebase
    // interacting with newer ones
    if( version > some_number )
      // do something
    else
      // do something else
  }
};

struct AnotherType { };

// Versioning can be applied to any type of serialization function, but
// if used for a load it must also be used for a save
template <class Archive>
void save( Archive & ar, AnotherType const & at, std::uint32_t const version )
{ }

template <class Archive>
void load( Archive & ar, AnotherType & at, std::uint32_t const version )
{ }

// Associate some type with a version number
CEREAL_CLASS_VERSION( MyCoolClass, 32 );

// If we don't associate a class with a version number and use a versioned
// serialize function, its version number will default to 0
```

<a name="inheritance"></a>
### Inheritance

Serialization functions, like any other function, will be inherited by derived classes.  Depending on the serialization
method you have chosen to employ, this can cause some ambiguities that cereal is not happy with:

```{cpp}
#include <iostream>
#include <cereal/archives/binary.hpp>

struct MyBase
{
  template <class Archive>
  void serialize( Archive & )
  { }
};

struct MyDerived : MyBase
{
  template <class Archive>
  void load( Archive & )
  { }

  template <class Archive>
  void save( Archive & ) const
  { }
};

int main()
{
  MyDerived d;
  cereal::BinaryOutputArchive ar( std::cout );
  ar( d ); // static assertion failure: 
           // xCEREAL detected both a serialize and save/load pair for MyDerived
}
```

In the above example, `MyDerived` inherits the public `serialize` function from `MyBase`, thus giving it both a
`serialize` and `load`-`save` pair.  cereal is unable to disambiguate which it should call and thus gives a compile time
static assertion.  

Even if `serialize` was made `private` in the base class, cereal may still have access to it from
the derived class because of a friend declaration to `cereal::access`, resulting in the same error.  The solution to this
error is to provide an explicit disambiguation for cereal:

```{cpp}
#include <cereal/access.hpp>

namespace cereal
{
  template <class Archive> 
  struct specialize<Archive, MyDerived, cereal::specialization::member_load_save> {};
  // xCEREAL no longer has any ambiguity when serializing MyDerived
}

// alternatively, you can use a macro at global scope to do the above disambiguation:
// CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES( MyDerived, cereal::specialization::member_load_save )

```

More information can be found by reading the doxygen documentation for `<cereal/access.hpp>` [here]({{ site.baseurl }}/assets/doxygen/structcereal_1_1specialize.html).

### Default construction

cereal requires access to a default constructor for types it serializes.  If you don't want to provide a default
constructor but do want to serialize smart pointers to it, you can get around this restriction using a special overload,
detailed in the [pointers](pointers) section of the documentation.
