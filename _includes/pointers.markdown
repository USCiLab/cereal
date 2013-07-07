Pointers
========

cereal supports serializing smart pointers but not dumb pointers (that is to say raw pointers, such as `int *`).  Pointer support can be found by including `<cereal/types/memory.hpp>`

---

### TLDR Version

cereal works with modern smart pointers found in `<memory>` by including `<cereal/types/memory.hpp>` but does not support raw pointers.  Pointers to polymorphic types are also supported but detailed [elsewhere](polymorphism.html).  cereal needs access to either a default constructor or a a specialization of `cereal::LoadAndAllocate` for loading smart pointers.

---

## Smart Pointers

All smart pointer types in C++11, `std::shared_ptr`, `std::unique_ptr`, and `std::weak_ptr` can be serialized by cereal.  `std::auto_ptr` is not supported (and is deprecated anyway - don't use it!).

cereal will make sure that the data pointed to by an `std::shared_ptr` is serialized only once, even if several `std::shared_ptr` (or `std::weak_ptr`) that point to the same data are serialized in the same archive.  This means that when saving to an archive cereal will avoid duplication, and when loading from an archive, cereal will not allocate extraneous data.

cereal will also properly handle pointers to polymorphic objects (e.g. `std::unique_ptr<Base> p( new Derived() )`.  This is detailed in the [polymorphic](polymorphism.html) wiki page.

---

### Types with no default constructor

If you want to serialize a pointer to a type that does not have a default constructor, or a type that does not allow cereal access to its default constructor, you will need to provide a specialization of `cereal::LoadAndAllocate` for the type.  cereal will call the static method of this struct, `load_and_allocate`, which will be called instead of its normal serialization functionality to dynamically allocate a new instance of the type and load all of its contents.  Don't worry about returning the raw pointer here - cereal will contain it within a smart pointer.

```cpp
#include <cereal/access.hpp> // For LoadAndAllocate

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
};

// Provide a specialization for LoadAndAllocate for your type
namespace cereal
{
  template <> struct LoadAndAllocate<MyType>
  {
    // load_and_allocate will be passed the archive that you will be loading
    // from and should return a raw pointer to a dynamically allocated instance
    // of your type.
    //
    // This will be captured by a smart pointer of some type and you need not
    // worry about managing the memory
    template <class Archive>
    static MyType * load_and_allocate( Archive & ar )
    {
      int x;
      ar( x );
      return new MyType( x );
    }
  };
```

---

### Implementation notes

When saving an `std::shared_ptr`, we first check to make sure we haven't serialized it before.  This is done by keeping a map from addresses to pointer ids (an `std::uint32_t`), which are unique.  Pointers that are newly serialized are given a new id with the most significant bit set to `1`.  When saved, an `std::shared_ptr` will first output its id, which will either have its MSB set to `1` if it is the first instance of that id, or will be an id already in the archive.  This is immediately followed by the data found by dereferencing the pointer.  If the pointer was equal to `nullptr`, its id is set to `0` and nothing else is saved.

When loading an `std::shared_ptr`, we first load its id to determine whether we need to allocate data or not.  If the MSB of a loaded id is equal to `1`, we need to actually allocate it and keep track of this allocation in a map from ids to `std::shared_ptr<void>`.  If the MSB is not equal to `1`, we retrieve the already loaded data from the map.  In the case of null pointers (id equal to `0`), nothing needs to be done.

When serializing an `std::weak_ptr`, it is promoted to an `std::shared_ptr` by locking it.  It then follows the same mechanisms as a shared pointer.

`std::unique_ptr` is a bit easier since it contains a unique reference to the data it points to.  This means we can serialize it without doing any tracking.  The only extra metadata that we serialize is a single `std::uint8_t` containing a `1` if the pointer is valid and a `0` if the pointer is equal to `nullptr`.  When loading, we first look at the id before attempting to load its contents.

---

## Raw/Dumb Pointers

cereal does not support serializing raw (e.g. `int *`) pointers.

---

### Pointer serialization rationale

Pointer serialization requires extra book-keeping compared to non pointer types.  We want to make sure that we don't serialize the same object twice since that is both time and space wasted in the archive.  With raw pointers, every object would need to be tracked to detect whether it had already been serialized (imagine a pointer pointing to data inside of some other container - this gets complicated very quickly).  This is essentially what happens with boost serialization.  This comes at the cost of runtime performance and increases the code complexity of the library considerably.  In modern C++, it is rare for raw pointers to be used outside of internal implementations, so we think that in these situations it is reasonable to make users of cereal make their own decisions regarding the data the pointers refer to.
