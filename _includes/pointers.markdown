Pointers
========

cereal supports serializing smart pointers but not dumb pointers (that is to say raw pointers, such as `int *`) or references.  Pointer support can be found by including `<cereal/types/memory.hpp>`

---

### TLDR Version

cereal works with modern smart pointers found in `<memory>` by including `<cereal/types/memory.hpp>` but does not support raw pointers or references.  Pointers to polymorphic types are also supported but detailed [elsewhere](polymorphism.html).  cereal needs access to either a default constructor or a a specialization of `cereal::LoadAndConstruct` for loading smart pointers.

---

## Smart Pointers

All smart pointer types in C++11, `std::shared_ptr`, `std::unique_ptr`, and `std::weak_ptr` can be serialized by cereal.  In addition, cereal also supports `std::enable_shared_from_this`, as well as needlessly convoluted circular references of `std::weak_ptr`.  `std::auto_ptr` is not supported (and is deprecated anyway - don't use it!).

cereal will make sure that the data pointed to by an `std::shared_ptr` is serialized only once, even if several `std::shared_ptr` (or `std::weak_ptr`) that point to the same data are serialized in the same archive.  This means that when saving to an archive cereal will avoid duplication, and when loading from an archive, cereal will not allocate extraneous data.

cereal will also properly handle pointers to polymorphic objects (e.g. `std::unique_ptr<Base> p( new Derived() )`.  This is detailed in the [polymorphic](polymorphism.html) wiki page.

### Types with no default constructor

If you want to serialize a pointer to a type that does not have a default constructor, or a type that does not allow cereal access to its default constructor, you will either need to provide a member static function `load_and_construct` or provide a specialization of `cereal::LoadAndConstruct` for the type.  If you choose to use the external version, specializing `cereal::LoadAndConstruct`, cereal will call the static method of this struct, `load_and_construct`.

These functions give you a way to load up any pertinent data and call a non default constructor.  cereal will handle
allocating the memory and pass a `cereal::construct` object to your `load_and_construct` function that you can use to
perform the actual construction.

```cpp
#include <cereal/access.hpp> // For LoadAndConstruct

struct MyType
{
  MyType( int x ); // note: no default ctor
  int myX;

  // Define a serialize or save/load pair as you normally would
  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( myX );
  }

  // We could define load_and_construct internally
  //static void load_and_construct( Archive & ar, cereal::construct<MyType> & construct )
  //{
  //  int x;
  //  ar( x );
  //  construct( x );
  //}
};

// Provide a specialization for LoadAndConstruct for your type
// if you want to do it externally.
namespace cereal
{
  template <> struct LoadAndConstruct<MyType>
  {
    // load_and_construct will be passed the archive that you will be loading
    // from as well as a special construct object that can be used to invoke
    // the constructor for your class.
    //
    // More advanced functionality is available using construct, such as accessing
    // class members, which is detailed in the doxygen docs.
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<MyType> & construct )
    {
      int x;
      ar( x );
      construct( x ); // calls MyType( x )
    }
  };
```

### Implementation notes

If you are a casual user of cereal, there's no need to worry about the details of loading and saving smart pointers.  If you are curious or want a
better understanding, here's a brief overview:

When saving an `std::shared_ptr`, we first check to make sure we haven't serialized it before.  This is done by keeping a map from addresses to pointer ids (an `std::uint32_t`), which are unique.  Pointers that are newly serialized are given a new id with the most significant bit set to `1`.  When saved, an `std::shared_ptr` will first output its id, which will either have its MSB set to `1` if it is the first instance of that id, or will be an id already in the archive.  This is immediately followed by the data found by dereferencing the pointer.  If the pointer was equal to `nullptr`, its id is set to `0` and nothing else is saved.

When loading an `std::shared_ptr`, we first load its id to determine whether we need to allocate data or not.  If the MSB of a loaded id is equal to `1`, we need to actually allocate it and keep track of this allocation in a map from ids to `std::shared_ptr<void>`.  If the MSB is not equal to `1`, we retrieve the already loaded data from the map.  In the case of null pointers (id equal to `0`), nothing needs to be done.

When serializing an `std::weak_ptr`, it is promoted to an `std::shared_ptr` by locking it.  It then follows the same mechanisms as a shared pointer.

`std::unique_ptr` is a bit easier since it contains a unique reference to the data it points to.  This means we can serialize it without doing any tracking.  The only extra metadata that we serialize is a single `std::uint8_t` containing a `1` if the pointer is valid and a `0` if the pointer is equal to `nullptr`.  When loading, we first look at the id before attempting to load its contents.

Handling of types that do not have default constructors works the same way on the surface but is more complicated under
the hood.  cereal allocates memory for the type before it can be constructed and gives the user an object which
transparently performs a placement new into this block of memory, while staying exception safe.

The most complicated portion of the memory implementation is handling non-default constructors when an `std::shared_ptr`
to a type that inherits from `std::enable_shared_from_this` is utilized (excluding the complications that arise from polymorphism!).  In this situation, we need to pre-allocate a buffer for the user to later perform a placement new in, while keeping a stable, type agnostic `std::shared_ptr` to the buffer (so we can properly assign circular references).  The internal state of the `std::enable_shared_from_this` is set upon insertion into a `std::shared_ptr`, which means that if we were to simply let user perform their placement new blindly, the original state of the `std::enable_shared_from_this` would be overwritten.  To get around this, we save its state before letting the user peform the construction, and then restore it afterwards.

---

## Raw/Dumb Pointers and References

cereal does not support serializing raw (e.g. `int *`) pointers or references (e.g. `int &`).

### Pointer serialization rationale

Pointer serialization requires extra book-keeping compared to non pointer types.  We want to make sure that we don't serialize the same object twice since that is both time and space wasted in the archive.  With raw pointers, every object would need to be tracked to detect whether it had already been serialized (imagine a pointer pointing to data inside of some other container - this gets complicated very quickly).  This is essentially what happens with boost serialization.  This comes at the cost of runtime performance and increases the code complexity of the library considerably.  It also complicates user code since it adds extra considerations about tracking types.  In modern C++, it is rare for raw pointers to be used outside of internal implementations, so we think that in these situations it is reasonable to make users of cereal make their own decisions regarding the data the pointers refer to.
