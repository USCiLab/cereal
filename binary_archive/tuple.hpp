#ifndef CEREAL_BINARY_ARCHIVE_TUPLE_HPP_
#define CEREAL_BINARY_ARCHIVE_TUPLE_HPP_

#include <cereal/details/traits.hpp>
#include <cereal/binary_archive/binary_archive.hpp>
#include <tuple>

namespace cereal
{
  namespace tuple_detail
  {
    // unwinds a tuple to save it
    template <size_t Height>
    struct serialize
    {
      template <class Archive, class ... Types> inline
      static void apply( Archive & ar, std::tuple<Types...> & tuple )
      {
        ar & std::get<Height - 1>( tuple );
        serialize<Height - 1>::template apply( ar, tuple );
      }
    };

    // Zero height specialization - nothing to do here
    template <>
    struct serialize<0>
    {
      template <class Archive, class ... Types> inline
      static void apply( Archive & ar, std::tuple<Types...> & tuple )
      { }
    };
  }

  //! Serializing for std::tuple to binary
  template <class Archive, class ... Types> inline
  CEREAL_ARCHIVE_RESTRICT_SERIALIZE(BinaryInputArchive, BinaryOutputArchive)
  serialize( Archive & ar, std::tuple<Types...> & tuple )
  {
    tuple_detail::serialize<std::tuple_size<std::tuple<Types...>>::value>::template apply( ar, tuple );
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_TUPLE_HPP_
