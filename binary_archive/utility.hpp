#ifndef CEREAL_BINARY_ARCHIVE_UTILITY_HPP_
#define CEREAL_BINARY_ARCHIVE_UTILITY_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <utility>

namespace cereal
{
  //! Serializing for std::pair to binary
  template <class Archive, class T1, class T2> inline
  void serialize( Archive & ar, std::pair<T1, T2> & pair )
  {
    ar & pair.first;
    ar & pair.second;
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_UTILITY_HPP_
