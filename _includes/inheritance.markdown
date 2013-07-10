Inheritance
===========

cereal fully supports serializing classes that use inheritance.  This page is concerned with how to serialize base classes from a derived class.  Another important and related feature of inheritance, polymorphism, is discussed [elsewhere](polymorphism.html).

---

### TLDR Version

Archive casted versions of derived classes within your serialize functions instead of calling serialize functions directly.  Use `cereal::base_class<BaseT>( this )` for non-virtual inheritance, and `cereal::virtual_base_class<BaseT>( this )` for virtual inheritance.

---

## Normal Inheritance

When inheriting from objects without using virtual inheritance (e.g. `struct Derived : public Base`), the recommended method is to utilize `cereal::base_class` to cast the derived class to the base class.  This is the preferred method over calling a base class's serialize function directly since it may have any one of four different methods for serialization (see [serialization functions](serialization_functions.html)).  This is also preferred over using `static_cast`, since static casting will not work in situations where the base class is abstract.  Here is an example of how to do this:

```cpp
#include <cereal/types/base_class.hpp>

struct Base
{
  int x;
  // Note the non-member serialize - trying to call serialize
  // from a derived class wouldn't work
};

template <class Archive>
void serialize( Archive & ar, Base & b )
{ ar( b.x ); }

struct Derived : public Base
{
  int y;
  template <class Archive>
  void serialize( Archive & ar )
  { 
    // We pass this cast to the base type for each base type we
    // need to serialize.  Do this instead of calling serialize functions
    // directly
    ar( cereal::base_class<Base>( this ), y ); 
  }
};
```

---

## Virtual Inheritance
In virtual inheritance, only one copy of each base class will be contained within a derived object.  cereal provides a method for tracking virtual base classes, preventing multiple copies from being serialized.  If `cereal::base_class` were used in a situation where the same base class appeared in more than one traversal of the class hierarchy, it could potentially result in duplicate information being serialized.  To prevent this, use `cereal::virtual_base_class`, which lets cereal track the instantiation of base objects for a derived class, ensuring that only one copy of each base class is serialized.

```cpp
#include <cereal/types/base_class.hpp>

struct Base
{
  int x;
  // Note the non-member serialize - trying to call serialize
  // from a derived class wouldn't work
};

template <class Archive>
void serialize( Archive & ar, Base & b )
{ ar( b.x ); }

struct Left : virtual Base
{
  int l;

  template <class Archive>
  void serialize( Archive & ar )
  { 
    ar( cereal::virtual_base_class<Base>( this ), l );
  }
};

struct Right : virtual Base
{
  int r;

  template <class Archive>
  void serialize( Archive & ar )
  { 
    ar( cereal::virtual_base_class<Base>( this ), r );
  }
};

struct Derived : virtual Left, virtual Right
{
  int y;
  template <class Archive>
  void serialize( Archive & ar )
  { 
    // Since we've used virtual inheritance and virtual_base_class,
    // xCEREAL will ensure that only one copy of each base class
    // is serialized
    ar( cereal::virtual_base_class<Base>( this ), y ); 
  }
};

// With virtual inheritance and cereal::virtual_base_class, x, l, r, and y will be serialized exactly once.
// If we had not used virtual_base_class, two copies of the base class may have been serialized,
// resulting in duplicate x entries.
```

