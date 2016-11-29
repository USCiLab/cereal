/*
Copyright (c) 2014, Steve Hickman
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of cereal nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STEVE HICKMAN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "common.hpp"
#include <boost/test/unit_test.hpp>


using namespace boost::filesystem;

template <class IArchive, class OArchive>
void test_boost_filesystem_path()
{
   // Start with the current directory and work up.
   path cwd(".");

   std::vector<path> o_paths;
   path current = canonical(cwd);
   while (!current.empty())
   {
      o_paths.push_back(current);
      current = current.parent_path();
   }

   std::ostringstream os;
   {
      OArchive oar(os);

      oar(o_paths);

   }

   std::vector<path> i_paths;
   std::istringstream is(os.str());
   {
      IArchive iar(is);

      iar(i_paths);
   }

   BOOST_CHECK_NE(i_paths.size(), 0);
   BOOST_CHECK_EQUAL(i_paths.size(), o_paths.size());
   auto o_v_it = o_paths.begin();
   auto i_v_it = i_paths.begin();
   for (; o_v_it != o_paths.end(); ++o_v_it, ++i_v_it)
   {
      // check if query methods match
      BOOST_CHECK_EQUAL(i_v_it->empty(), o_v_it->empty());
      BOOST_CHECK_EQUAL(i_v_it->has_root_name(), o_v_it->has_root_name());
      BOOST_CHECK_EQUAL(i_v_it->has_root_directory(), o_v_it->has_root_directory());
      BOOST_CHECK_EQUAL(i_v_it->has_root_path(), o_v_it->has_root_path());
      BOOST_CHECK_EQUAL(i_v_it->has_relative_path(), o_v_it->has_relative_path());
      BOOST_CHECK_EQUAL(i_v_it->has_parent_path(), o_v_it->has_parent_path());
      BOOST_CHECK_EQUAL(i_v_it->has_filename(), o_v_it->has_filename());
      BOOST_CHECK_EQUAL(i_v_it->has_stem(), o_v_it->has_stem());
      BOOST_CHECK_EQUAL(i_v_it->has_extension(), o_v_it->has_extension());
      BOOST_CHECK_EQUAL(i_v_it->is_absolute(), o_v_it->is_absolute());
      BOOST_CHECK_EQUAL(i_v_it->is_relative(), o_v_it->is_relative());

      // check if decomposition methods match
      BOOST_CHECK_EQUAL(i_v_it->root_name(), o_v_it->root_name());
      BOOST_CHECK_EQUAL(i_v_it->root_directory(), o_v_it->root_directory());
      BOOST_CHECK_EQUAL(i_v_it->root_path(), o_v_it->root_path());
      BOOST_CHECK_EQUAL(i_v_it->relative_path(), o_v_it->relative_path());
      BOOST_CHECK_EQUAL(i_v_it->parent_path(), o_v_it->parent_path());
      BOOST_CHECK_EQUAL(i_v_it->filename(), o_v_it->filename());
      BOOST_CHECK_EQUAL(i_v_it->stem(), o_v_it->stem());
      BOOST_CHECK_EQUAL(i_v_it->extension(), o_v_it->extension());

   }
}

template <class IArchive, class OArchive>
void test_boost_filesystem_file_status()
{
   // Create every combination of individual values
   std::vector<file_status> o_individual_statuses;
   file_type ftypes[] = { status_error, file_not_found, regular_file, directory_file, symlink_file, block_file, character_file, fifo_file, socket_file, type_unknown };
   perms permissions[] = { no_perms,
      owner_read, owner_write, owner_exe, owner_all,
      group_read, group_write, group_exe, group_all,
      others_read, others_write, others_exe, others_all,
      all_all,
      set_uid_on_exe, set_gid_on_exe,
      sticky_bit,
      perms_mask, perms_not_known, add_perms, remove_perms, symlink_perms };

   for (int i = 0; i < sizeof(ftypes); i++)
   {
      for (int j = 0; j < sizeof(permissions); j++)
      {
         o_individual_statuses.push_back(file_status(ftypes[i], permissions[j]));
      }
   }


   std::ostringstream os;
   {
      OArchive oar(os);

      oar(o_individual_statuses);

   }

   std::vector<file_status> i_individual_statuses;
   std::istringstream is(os.str());
   {
      IArchive iar(is);

      iar(i_individual_statuses);
   }

   BOOST_CHECK_NE(i_individual_statuses.size(), 0);
   BOOST_CHECK_EQUAL(i_individual_statuses.size(), o_individual_statuses.size());
   auto o_v_it = o_individual_statuses.begin();
   auto i_v_it = i_individual_statuses.begin();
   for (; o_v_it != o_individual_statuses.end(); ++o_v_it, ++i_v_it)
   {
      // check if query methods match
      BOOST_CHECK_EQUAL(i_v_it->type(), o_v_it->type());
      BOOST_CHECK_EQUAL(i_v_it->permissions(), o_v_it->permissions());

   }
}

template <class IArchive, class OArchive>
void test_boost_filesystem_directory_entry()
{
   // Start with the current directory and work up.
   path cwd(".");

   std::vector<directory_entry> o_dirs;
   directory_entry current(canonical(cwd));
   while (!current.path().empty())
   {
      o_dirs.push_back(current);
      current.assign(current.path().parent_path());
   }

   std::ostringstream os;
   {
      OArchive oar(os);

      oar(o_dirs);

   }

   std::vector<directory_entry> i_dirs;
   std::istringstream is(os.str());
   {
      IArchive iar(is);

      iar(i_dirs);
   }

   BOOST_CHECK_NE(i_dirs.size(), 0);
   BOOST_CHECK_EQUAL(i_dirs.size(), o_dirs.size());
   auto o_v_it = o_dirs.begin();
   auto i_v_it = i_dirs.begin();
   for (; o_v_it != o_dirs.end(); ++o_v_it, ++i_v_it)
   {
      // check if query methods match
      BOOST_CHECK_EQUAL(i_v_it->path(),                  o_v_it->path());
      BOOST_CHECK_EQUAL(i_v_it->status().type(),         o_v_it->status().type());
      BOOST_CHECK_EQUAL(i_v_it->status().permissions(),  o_v_it->status().permissions());
      BOOST_CHECK_EQUAL(i_v_it->symlink_status().type(), o_v_it->symlink_status().type());
      BOOST_CHECK_EQUAL(i_v_it->symlink_status().permissions(), o_v_it->symlink_status().permissions());

   }
}
template <class IArchive, class OArchive>
void test_boost_filesystem()
{
   test_boost_filesystem_path<IArchive, OArchive>();
   test_boost_filesystem_file_status<IArchive, OArchive>();
   test_boost_filesystem_directory_entry<IArchive, OArchive>();

}

/// * * * * Cereal XML and JSON archives don't currently support wstring/wchar (Issue #95) * * * *
// When they do, we can use this:
////template <class IArchive, class OArchive>
//struct test_boost_filesystem
//{
//   template <typename ArchiveSet>
//   void operator()(ArchiveSet x)
//   {
//      BOOST_LOG_TRIVIAL(debug) << "\n...Starting test_boost_filesystem for " << typeid(ArchiveSet).name();
//
//      test_boost_filesystem_path<typename ArchiveSet::iarchive, typename ArchiveSet::oarchive>();
//      test_boost_filesystem_file_status<typename ArchiveSet::iarchive, typename ArchiveSet::oarchive>();
//      test_boost_filesystem_directory_entry<typename ArchiveSet::iarchive, typename ArchiveSet::oarchive>();
//
//   }
//};
//
//BOOST_AUTO_TEST_CASE(all_archives_boost_filesystem)
//{
//   boost::mpl::for_each<archive_type_list>(test_boost_filesystem());
//}
BOOST_AUTO_TEST_CASE(binary_filesystem)
{
   test_boost_filesystem<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

BOOST_AUTO_TEST_CASE(portable_binary_filesystem)
{
   test_boost_filesystem<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}


/// * * * * Cereal XML and JSON archives don't currently support wstring/wchar (Issue #95) * * * *
//BOOST_AUTO_TEST_CASE(xml_filesystem)
//{
//   test_boost_filesystem<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
//}

//BOOST_AUTO_TEST_CASE(json_filesystem)
//{
//   test_boost_filesystem<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
//}

