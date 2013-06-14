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
    template <size_t Height, class Archive, class ... Types> inline
    void serialize( Archive & ar, std::tuple<Types...> & tuple )
    {
      ar & std::get<Height - 1>( tuple );
      serialize<Height - 1>( ar, tuple );
    }

    // Zero height specialization - nothing to do here
    template <class Archive, class ... Types> inline
    void serialize<0, Types...>( Archive & ar, std::tuple<Types...> & tuple )
    { }
  }

  //! Serializing for std::tuple to binary
  template <class Archive, class ... Types> inline
  CEREAL_ARCHIVE_RESTRICT_SERIALIZE(BinaryInputArchive, BinaryOutputArchive)
  serialize( Archive & ar, std::tuple<Types...> & tuple )
  {
    tuple_size::serialize<std::tuple_size<std::tuple<Types...>>>( ar, tuple );
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_TUPLE_HPP_
