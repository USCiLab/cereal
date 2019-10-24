#include <base.hpp>
#include <derived.hpp>

#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/cereal.hpp>

#include <cereal/types/array.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>

#include <cereal/details/util.hpp>

#include <cereal/details/polymorphic_impl.hpp>
#include <cereal/types/polymorphic.hpp>

#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

#include <iostream>
#include <type_traits>
#include <functional>

//CEREAL_FORCE_LINK_SHARED_LIBRARY(Sandbox)

struct Archive {};
CEREAL_SETUP_ARCHIVE_TRAITS(Archive, Archive)

struct Test
{
  template <class Archive>
  void serialzize( Archive & )
  {
    std::cout << "hey there" << std::endl;
  }

  template <class Archive>
  void save( Archive & ) const
  {
    std::cout << "saved by the bell" << std::endl;
  }

  template <class Archive>
  void load( Archive & )
  {
    std::cout << "locked and loaded" << std::endl;
  }

  template <class Archive>
  static void load_and_construct( Archive &, cereal::construct<Test> & )
  {
  }

  template <class Archive>
  int save_minimal() const
  {
    return 0;
  }

  template <class Archive>
  int save_minimal(const std::uint32_t) const
  {
    return 1;
  }

  template <class Archive>
  void load_minimal( int & )
  { }
};

template <class Archive>
void serialize( Archive &, Test & )
{ }

template <class Archive>
void load( Archive &, Test & )
{ }

template <class Archive>
void save( Archive &, Test const & )
{ }

template <class Archive>
int save_minimal( Test const & )
{ return 0; }

template <class Archive>
int save_minimal( Test const &, const std::uint32_t )
{ return 0; }

namespace cereal
{
  template <>
  struct LoadAndConstruct<Test>
  {
    template <class Archive>
    static void load_and_construct( Archive &, cereal::construct<Test> & construct )
    {
      construct();
    }
  };
}

struct A
{
  virtual void foo() = 0;
  virtual ~A() {}
};

struct B : A
{
  virtual ~B() {}
  void foo() {}

  template <class Archive>
  void serialize( Archive & )
  {
    std::cout << "i'm in your b" << std::endl;
  }
};

struct C
{
  char a;
};

CEREAL_REGISTER_TYPE(B)
CEREAL_REGISTER_POLYMORPHIC_RELATION(A, B)

class MemberMinimal
{
  public:
    MemberMinimal() = default;
    template <class Archive>
    int save_minimal( Archive const & ) const
    {
      return x;
    }

    template <class Archive>
    void load_minimal( Archive const &, int const & str )
    {
      x = str;
    }

  public:
    int x;
};

int main()
{
  typedef Test T;
  std::cout << std::boolalpha;

  // Test Load and Construct internal/external
  std::cout << "\tload_and_construct" << std::endl;
  std::cout << cereal::traits::has_member_load_and_construct<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load_and_construct<T, Archive>::value << std::endl;

  // serialize
  std::cout << "\tserialize" << std::endl;
  std::cout << cereal::traits::has_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_serialize<T, Archive>::value << std::endl;

  // load
  std::cout << "\tload" << std::endl;
  std::cout << cereal::traits::has_member_load<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_load<T, Archive>::value << std::endl;

  // load minimal
  std::cout << "\tload minimal" << std::endl;
  std::cout << cereal::traits::has_member_load<T, Archive>::value << std::endl;

  // save
  std::cout << "\tsave" << std::endl;
  std::cout << cereal::traits::has_member_save<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_save<T, Archive>::value << std::endl;

  // save_minimal
  std::cout << "\tsave_minimal" << std::endl;
  std::cout << cereal::traits::has_member_save_minimal<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_save_minimal<T, Archive>::value << std::endl;

  // save_minimal_versioned
  std::cout << "\tsave_minimal versioned" << std::endl;
  std::cout << cereal::traits::has_member_versioned_save_minimal<T, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_versioned_save_minimal<T, Archive>::value << std::endl;

  // splittable
  std::cout << "\t splittable" << std::endl;
  std::cout << cereal::traits::has_member_split<T, Archive, Archive>::value << std::endl;
  std::cout << cereal::traits::has_non_member_split<T, Archive, Archive>::value << std::endl;

  // serialiable
  std::cout << "\toutput serializable" << std::endl;
  std::cout << cereal::traits::is_output_serializable<T, Archive>::value << std::endl;

#if !defined(__INTEL_COMPILER)
  //! TODO: This causes icc to crash
  std::cout << cereal::traits::is_input_serializable<T, Archive>::value << std::endl;
#endif

  // specialized
  std::cout << "\tspecialized" << std::endl;
  std::cout << cereal::traits::detail::is_specialized_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_member_load_save<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_non_member_serialize<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::is_specialized_non_member_load_save<T, Archive>::value << std::endl;
  std::cout << cereal::traits::detail::count_specializations<T, Archive>::value << std::endl;
  std::cout << cereal::traits::is_specialized<T, Archive>::value << std::endl;

  // array size
  std::cout << typeid(A).name() << std::endl;
  std::cout << typeid(cereal::traits::has_load_and_construct<int, bool>).name() << std::endl;

  // extra testing
  std::cout << "\textra" << std::endl;
  std::cout << cereal::traits::has_member_save_minimal<MemberMinimal, Archive>::value << std::endl;
  std::cout << cereal::traits::has_member_load_minimal<MemberMinimal, Archive>::value << std::endl;

  // DLL testing
  std::cout << "------DLL TESTING------" << std::endl;
  std::stringstream dllSS1;
  std::stringstream dllSS2;
  {
    cereal::XMLOutputArchive out(dllSS1);
    VersionTest x{1};
    std::shared_ptr<Base> p = std::make_shared<Derived>();
    out(x);
    out( p );

    std::shared_ptr<A> ay = std::make_shared<B>();
    out(ay);
  }

  std::cout << dllSS1.str() << std::endl;

  {
    VersionTest x;
    std::shared_ptr<Base> p;
    std::shared_ptr<A> ay;
    {
      cereal::XMLInputArchive in(dllSS1);
      in(x);
      in( p );
      in( ay );
    }
    {
      cereal::XMLOutputArchive out(dllSS2);
      out( x );
      out( p );
      out( ay );
    }
  }

  std::cout << dllSS2.str() << std::endl;

  return 0;
}
