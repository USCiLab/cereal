Standard Library Support
========================

cereal supports most of the containers found in the C++ standard library out of the box.

---

### TLDR Version

cereal supports pretty much everything in the C++ standard library.  Include the proper header from cereal to enable
support (e.g. `<cereal/types/vector.hpp>`.

---

## STL Support

To use a type found in the standard library, just include the proper header from `#include <cereal/types/xxxx.hpp>`.  See the [doxygen docs]({{ site.baseurl }}/assets/doxygen/group__STLSupport.html) for a complete list of supported types.
