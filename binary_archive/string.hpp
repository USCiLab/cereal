#ifndef CEREAL_BINARY_ARCHIVE_STRING_HPP_
#define CEREAL_BINARY_ARCHIVE_STRING_HPP_

#include <cereal/binary_archive/binary_archive.hpp>
#include <string>

namespace cereal
{
  //! Serialization for basic_string types to binary
  template<class CharT, class Traits, class Alloc>
  void save(BinaryOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
  {
    std::cout << "Saving string" << std::endl;

    // Save number of chars + the data
    ar & str.size();
    ar.save_binary(str.data(), str.size() * sizeof(CharT));
  }

  //! Serialization for basic_string types from binary
  template<class CharT, class Traits, class Alloc>
  void load(BinaryInputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str)
  {
    std::cout << "Loading string" << std::endl;

    size_t size;
    ar & size;
    str.resize(size);
    ar.load_binary(const_cast<CharT*>(str.data()), size * sizeof(CharT));
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_STRING_HPP_

