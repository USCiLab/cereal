Standard Library Support
========================

cereal supports most of the containers found in the C++ standard library out of the box.

---

### TLDR Version

cereal supports pretty much everything in the C++ standard library.  Include the proper header from cereal to enable
support (e.g. `<cereal/types/vector.hpp>`. See the [doxygen docs]({{ site.baseurl }}/assets/doxygen/group__STLSupport.html) for a complete list of supported types.

---

## STL Support

To use a type found in the standard library, just include the proper header from `#include <cereal/types/xxxx.hpp>` and
serialize data as you normally would:

```{cpp}
// type support
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>

// for doing the actual serialization
#include <cereal/archives/json.hpp>
#include <iostream>

class Stuff
{
  public:
    Stuff() = default;

    void fillData()
    {
      data = { {"real", { {1.0f, 0},
                          {2.2f, 0},
                          {3.3f, 0} }},
               {"imaginary", { {0, -1.0f},
                               {0, -2.9932f},
                               {0, -3.5f} }} };
    }

  private:
    std::map<std::string, std::vector<std::complex<float>>> data;
    
    friend class cereal::access;

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( CEREAL_NVP(data) );
    }
};

int main()
{
  cereal::JSONOutputArchive output(std::cout); // stream to cout

  Stuff myStuff;
  myStuff.fillData();

  output( cereal::make_nvp("best data ever", myStuff) );
}
```

which will produce the following JSON:

```{json}
{
    "best data ever": {
        "data": [
            {
                "key": "imaginary",
                "value": [
                    {
                        "real": 0,
                        "imag": -1
                    },
                    {
                        "real": 0,
                        "imag": -2.9932
                    },
                    {
                        "real": 0,
                        "imag": -3.5
                    }
                ]
            },
            {
                "key": "real",
                "value": [
                    {
                        "real": 1,
                        "imag": 0
                    },
                    {
                        "real": 2.2,
                        "imag": 0
                    },
                    {
                        "real": 3.3,
                        "imag": 0
                    }
                ]
            }
        ]
    }
}
```

More information on the archives and functions used in the above example can be found in the [serialization
functions](serialization_functions.html) and [serialization archives](serialization_archives.html) sections of the
documentation.
