Archive Specialization
============

cereal supports having specific serialization behaviors for different archive types.

---

### TLDR Version

You can specialize your own types or types that cereal comes with support for to exhibit specific behaviors with
specific archives.  This is typically done by replacing at least one generic templated parameter with the type you wish to specialize for.  Specializations will be given precedence over any templated serialization function.

If you want to specialize for several types of archives, you will need to use templates and type traits to restrict
template instantiation.  cereal provides several traits to help with this, defined in `<cereal/details/traits.hpp>`.

---

## Specializing Built-in Types

Although cereal comes with a support for nearly every type in the [standard library](stl_support.html), there can be
times when it is desirable to have custom functionality for a type that cereal provides the serialization for.  Function
overloading can be used to override the cereal implementation with a custom one.  This will work even if you include the
cereal support for a type assuming that the compiler doesn't find any ambiguity in your overload (more on that later).

### Specializing the archive

If you want to make an archive behave differently for some type, you can do this by creating overloads where the archive
is explicitly defined (and not a template parameter):

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
  // (i.e. void serialize()) because it expects to be called
  // for loading (an input archive) and saving (an output archive).
  // We would end up needing to implement two versions of it, so using a load/save
  // pair makes more sense.
}
```

<span class="label label-warning">Important!</span>
When overloading a cereal provided serialization function, you should place it in the `xCEREAL` namespace.

### Specializing the type

It is also easy to specialize the behavior of a specific type for all archives or a group of archives.  This is done by
restricting the serialized type instead of restricting the archive (though you can definitely specialize on both the
archive and the type).

The following example shows how to specialize serialization for `std::map<std::string, std::string>` for text archives such that it
roughly matches the output of an SQL \`WHERE\` clause in which all the expressions (from a name-value-pair perspective)
are ANDed together:

#### The serialization code:
```cpp
namespace cereal
{
  //! Saving for std::map<std::string, std::string> for text based archives
  // Note that this shows off some internal xCEREAL traits such as EnableIf,
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
  template <class Archive, class C, class A,
            traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
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
template parameter.  In the above examples we specialized two of the template parameters for `std::map`.

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

Specializing a user defined type works in almost exactly the same fashion to types in the standard library that cereal provides
the implementation for.  The only real difference is that there is no longer a requirement that the serialization code
reside in the `xCEREAL` namespace - it now goes wherever you would normally put your serialization code.

Since there is no provided implementation to conflict with, you have more control over the generality of your
implementations.  It is much easier to use templates to restrict how the serialization functions are instantiated, as
shown in the following example, which uses different serialization depending on whether a binary archive is used versus
some other type:

```cpp
struct Hello
{
  int x;
  
  // Enabled for text archives (e.g. XML, JSON)
  template <class Archive,
            cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
            = cereal::traits::sfinae>
  std::string save_minimal( Archive & ) const
  { 
    return std::to_string( x ) + "hello";
  }

  // Enabled for text archives (e.g. XML, JSON)
  template <class Archive,
            cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
            = cereal::traits::sfinae>
  void load_minimal( Archive const &, std::string const & str )
  {
    x = std::stoi( str.substr(0, 1) );
  }

  // Enabled for binary archives (e.g. binary, portable binary)
  template <class Archive,
            cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
            = cereal::traits::sfinae>
  int save_minimal( Archive & ) const
  { 
    return x; 
  }

  // Enabled for binary archives (e.g. binary, portable binary)
  template <class Archive,
            cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
            = cereal::traits::sfinae>
  void load_minimal( Archive const &, int const & xx )
  { 
    x = xx;
  }
};
```

## Useful Type Traits

There are quite a few useful type traits in `<cereal/details/traits.hpp>` that are especially useful for specializing
various serialization functions.

EnableIf and DisableIf

is_same_archive

is_text_archive

strip_minimal
