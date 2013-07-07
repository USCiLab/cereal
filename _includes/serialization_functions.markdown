Serialization Functions
=======================

Because C++ lacks 
<a href="http://en.wikipedia.org/wiki/Reflection_(computer_programming)">reflection</a>,
implementing serialization requires the user to specify which data members
should be serialized. cereal provides a number of ways to specify such
serialization functions for your classes, even if you don't have access to
their internals.

---

### TLDR Version

cereal supports serialization function definitions both inside and outside of
classes. Those declared inside of functions can be made private or protected by
making your class friends with `cereal::access`.

---

### Types of Serialization Functions

Serialization functions can either be internal or external, functionality can
either be in a single `serialize` function, or a split `save` and `load`
functions.  When possible, it is preferred to use a single internal `serialize`
method, though split methods can be used when it is necessary e.g. to
dynamically allocate memory upon loading a class.

<br/>

{% capture class_begin %}struct MyClass 
{
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
};{% endhighlight %}
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
  } {% endhighlight %}
  </div>
</div>

---

### No Default Constructors

If you don't want to provide a default constructor for your class but do want
to serialize smart pointers to it, you can define a `load_and_allocate` method
that tells cereal how to create your object.

```{cpp}
struct MyClass
{
  MyClass() = delete;         // No default constructor!
  MyClass(int x_) : x(x_) {}

  int x;

  template<class Archive>
    void serialize(Archive & archive)
    { archive( x ); }

  template<class Archive>
  static MyClass * load_and_allocate(Archive & archive)
  {
    // Load the necessary data from the archive
    int x_;
    ar( x_ );

    // Instantiate the class with the loaded data
    return new MyClass(x_);
  }
};
```

<br/>

You can even create `load_and_allocate` methods externallly, thought the syntax
is a bit more verbose:

```{cpp}
struct MyClass
{
  MyClass() = delete;
  MyClass(int x_) : x(x_) {}

  int x;
};

namespace cereal
{
  template <>
  static MyClass * load_and_allocate<MyClass>(Archive & archive)
  {
    int x_;
    ar( x_ );

    return new MyClass(x_);
  }
}
```
