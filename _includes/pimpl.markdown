PIMPL Idiom
============

cereal can be used in classes with pointers to private implementations (PIMPL idiom).

---

### TLDR Version

cereal can be used with the PIMPL idiom so long as:


1. Serialization function definitions are deferred until after the PIMPL class is declared.
2. Serialization functions are explicitly instantiated for all potential archive types they will be used with.

---

## Writing Serialization Functions with the PIMPL Idiom

The [PIMPL idiom](https://en.wikipedia.org/wiki/Opaque_pointer) is a programming technique that hides the
implementation details of a class from its interface. It is often used to hide internal implementation details or data
from the user of a class by only exposing a pointer to an internal class which is defined in a source file inaccessible
to users.

cereal can be used with this idiom as demonstrated in the following example:

##### myclass.hpp
```cpp
#include <memory>
#include <cereal/access.hpp>

class MyClass
{
  public:
    MyClass();
    ~MyClass();

  private:
    int x;
    class MyClassDetail; // forward declaration of PIMPL class

    std::unique_ptr<MyClassDetail> impl;

    friend class cereal::access;

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void serialize( Archive & ar );
};

```

##### myclass.cpp
```cpp
#include "myclass.hpp"
#include <cereal/types/memory.hpp>

// We need to include all archives that this type will
// be serialized with for explicit instantiation
#include <cereal/archives/json.hpp>

// Definition for PIMPL class
class MyClass::MyClassDetail
{
  public:
    MyClassDetail() : secretData( 3.14 )
    { }

    double secretData;

  private:
    friend class cereal::access;

    template <class Archive>
    void load( Archive & ar )
    { ar( CEREAL_NVP(secretData) ); }

    template <class Archive>
    void save( Archive & ar ) const
    { ar( CEREAL_NVP(secretData) ); }
};

// We can now define the serialization function
template <class Archive> inline
void MyClass::serialize( Archive & ar )
{
  ar( CEREAL_NVP(x), CEREAL_NVP(impl) );
}

MyClass::MyClass() : x(1), impl( new MyClassDetail() )
{ }

MyClass::~MyClass() = default;

// We must also explicitly instantiate our template
// functions for serialization
template void MyClass::MyClassDetail::save<cereal::JSONOutputArchive>( cereal::JSONOutputArchive & ) const;
template void MyClass::MyClassDetail::load<cereal::JSONInputArchive>( cereal::JSONInputArchive & );

// Note that we need to instantiate for both loading and saving, even
// if we use a single serialize function
template void MyClass::serialize<cereal::JSONOutputArchive>( cereal::JSONOutputArchive & );
template void MyClass::serialize<cereal::JSONInputArchive>( cereal::JSONInputArchive & );
```

##### main.cpp
```cpp
#include "myclass.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#include <sstream>
#include <fstream>
#include <iostream>

int main()
{
  MyClass m;
  std::stringstream ss;

  {
    cereal::JSONOutputArchive archive( ss );
    archive( m );
  }

  std::cout << ss.str() << std::endl;

  {
    cereal::JSONInputArchive archive( ss );
    archive( m );
  }

  {
    std::ofstream os("out.cereal", std::ios::binary);
    cereal::BinaryOutputArchive archive( os );

    // The following will give us a linker error because
    // we did not explicitly instantiate our serialization
    // functions for binary archives
    //archive( m );
  }

  return 0;
}
```

Which gives as output:

```json
{
    "value0": {
        "x": 1,
        "impl": {
            "ptr_wrapper": {
                "valid": 1,
                "data": {
                    "secretData": 3.14
                }
            }
        }
    }
}
```

The key takeaway here is that the definition of the serialization function is deferred until after the declaration of
the PIMPL class. Since serialization functions usually accept template parameters, this means that explicit template
instantiation is required as the definition will be placed in a source file and be inaccessible through the header file.
A consequence of this is that you need to include and specialize your serialization functions for every archive type you
intend to serialize them with.
