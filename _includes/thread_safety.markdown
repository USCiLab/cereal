Thread Safety
============

cereal can be used in a multithreaded environment with some restrictions and precautions, detailed here.

---

### TLDR Version

If you want to be thread safe:

1. Ensure that archives are accessed by only one thread at a time.
2. If you will be accessing separate archives simultaneously, ensure `CEREAL_THREAD_SAFE` is defined and non-zero before
   any cereal headers are included, or modify its default value in `<cereal/macros.hpp>`.

---

## Using Threads Safely

cereal can be used safely with threads with minimal limitations. The most important limitation to be aware of is that
individual serialization archives are not designed to be accessed simultaneously from multiple threads. cereal expects that access
to an archive will happen in a serial manner and may have undefined behavior if an archive is accessed in parallel. 

### Using multiple archives in parallel

While a single archive should not be used in parallel, it is possible to use multiple distinct archives simultaneously.

cereal uses global objects to track metadata associated with polymorphism and versioning for serialization.
To keep cereal as light and fast as possible, its default behavior is to assume a single-threaded environment and
perform no locking. If you have the need to access distinct archives simultaneously, you will need to define the
`CEREAL_THREAD_SAFE` macro to be non-zero:

```cpp
// Before including any xCEREAL header file
#define CEREAL_THREAD_SAFE = 1

// Now include your xCEREAL headers
#include <cereal/cereal.hpp> 
// etc
```

This macro can be found in `<cereal/macros.hpp>` and is detailed in the [doxygen documentation]({{ site.baseurl
}}/assets/doxygen/polymorphic_8hpp.html).  Defining `CEREAL_THREAD_SAFE=1` will cause cereal to perform locking on
certain global objects used during polymorphism and versioning serialization. This will come with the performance
penalties associated with locking a mutex.

Note that you will need to ensure this macro is defined before any cereal headers are included in every compilation
unit. Alternatively, you can modify the default value for the macro in `<cereal/macros.hpp>` to avoid having to define
it manually.

If using cereal through [CMake](http://cmake.org), the `THREAD_SAFE` option can be used to add this definition
automatically.
