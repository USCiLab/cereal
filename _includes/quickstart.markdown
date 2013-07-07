Quick Start
===========

---

### Get cereal

cereal was designed to be included in your project, so just grab the latest
version from [Github] (https://github.com/USCiLab/cereal) and drop it somewhere
your project can find it.

---

### Add Serialization Methods for Your Classes

cereal has to know what data members to serialize in your classes. Let it know
by implementing one of the following types of serialization methods.

<br/>

{% capture class_begin %}struct MyClass {
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
};
  {% endhighlight %}
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
    void load(Archive & archive)
              MyClass & m)
    {
      archive( m.x, m.y, m.z ); 
    }
  {% endhighlight %}
  </div>
</div>

Internal serialization functions can also be made private or protected by adding `friend class cereal::access;` to your class.

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


