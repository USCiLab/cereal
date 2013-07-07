Quick Start
===========

---

### Get cereal

cereal was designed to be included in your project, so just grab the latest
version from [Github] (https://github.com/USCiLab/cereal) and drop it somewhere
your project can find.

---

### Add Serialization Methods for Your Classes

cereal has to know which data members to serialize in your classes. Let it know
by implementing a `serialize` method in your class.

```{cpp}
struct MyClass
{
  int x, y, z;

  // This method lets cereal know which data members to serialize
  template<class Archive>
    void serialize(Archive & archive)
    {
      archive( x, y, z );
    }
};
```

---

### Choose an archive

cereal currently supports three archive types: 
[binary] (serialization_archives.html#binary_archive), 
[XML] (serialization_archives.html#xml_archive), and 
[JSON] (serialization_archives.html#json_archive).

Include your preferred archive type with:  

`#include <cereal/archives/binary.hpp>`

`#include <cereal/archives/xml.hpp>`

or

`#include <cereal/archives/json.hpp>`

---

### Serialize your data

```{cpp}
#include <cereal/archives/binary.hpp>
#include <fstream>

int main()
{
  {
    std::ofstream os("data.cereal"); // Open a file for writing
    cereal::BinaryOutputArchive oarchive(os); // Create an output archive

    MyData m1, m2, m3;
    oarchive(m1, m2, m3); // Write the data to the archive
  }

  {
    std::ifstream is("data.cereal"); // Open a file for reading
    cereal::BinaryInputArchive iarchive(is); // Create an input archive

    MyData m1, m2, m3;
    iarchive(m1, m2, m3); // Read the data from the archive
  }
}
```

### Learn more

cereal can handle much more complex examples than the one above. Learn the ins
and outs of the library by checking out the links on the left.
