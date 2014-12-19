#ifndef CEREAL_MULTI_REGISTRATION_2_ARCHIVE_HPP_
#define CEREAL_MULTI_REGISTRATION_2_ARCHIVE_HPP_

#include "common.hpp"

template <template <typename> class ArchiveBaseT, typename Derived>
class TestArchiveT : public ArchiveBaseT<Derived>
{
public:
  template <typename Stream>
  TestArchiveT(Derived* derived, Stream& stream)
      : ArchiveBaseT<Derived>(derived, stream)
  {
  }
};

template <template <typename D> class BaseArchiveT>
using TestArchive = cereal::ConcreteArchiveWrapper<TestArchiveT, BaseArchiveT>;

template <template <typename D> class BaseArchiveT>
void save(TestArchive<BaseArchiveT>&, const TestTypeBase&)
{
}

template <template <typename D> class BaseArchiveT>
void load(TestArchive<BaseArchiveT>&, TestTypeBase&)
{
}

template <template <typename D> class BaseArchiveT>
void save(TestArchive<BaseArchiveT>& archive, const TestType& t)
{
  std::string tmpStr = t.str + ", specially saved";
  archive(tmpStr);
}

template <template <typename D> class BaseArchiveT>
void load(TestArchive<BaseArchiveT>& archive, TestType& t)
{
  archive(t.str);
  t.str += ", specially loaded";
}

CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::XMLInputArchiveT>)
CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::JSONInputArchiveT>)
CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::BinaryInputArchiveT>)
CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::PortableBinaryInputArchiveT>)

CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::XMLOutputArchiveT>)
CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::JSONOutputArchiveT>)
CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::BinaryOutputArchiveT>)
CEREAL_REGISTER_ARCHIVE(TestArchive<cereal::PortableBinaryOutputArchiveT>)

#endif // CEREAL_MULTI_REGISTRATION_2_ARCHIVE_HPP_
