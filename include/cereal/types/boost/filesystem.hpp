/*! \file boost_optional.hpp
\brief Support for boost::optional
\ingroup OtherTypes */
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
#ifndef CEREAL_TYPES_BOOST_FILESYSTEM_HPP_
#define CEREAL_TYPES_BOOST_FILESYSTEM_HPP_

#include <cereal/cereal.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace cereal
{
   //! Saving for boost::filesystem::path
   template <class Archive> inline
      void save(Archive & ar, ::boost::filesystem::path const & p)
   {
		ar( CEREAL_NVP_("path",  p.native()));

   }

   //! Loading for boost::filesystem::path
   template <class Archive> inline
      void load(Archive & ar, ::boost::filesystem::path & p)
   {
		::boost::filesystem::path::string_type s;
		ar( CEREAL_NVP_("path",  s));
		p = s;
   }
   
      //! Saving for boost::filesystem::filesystem_error 
   template <class Archive> inline
      void save(Archive & ar, ::boost::filesystem::filesystem_error  const & err)
   {
		ar( CEREAL_NVP_("ec",  err.code()));
		std::string what(err.what());
		ar( CEREAL_NVP_("what",  what));		
		ar( CEREAL_NVP_("path1",  err.path1()));
		ar( CEREAL_NVP_("path2",  err.path2()));

   }

   //! Loading for boost::filesystem::filesystem_error 
   template <class Archive> inline
      void load(Archive & ar, ::boost::filesystem::filesystem_error  & err)
   {
		system::error_code ec;
		std::string what;
		::boost::filesystem::path  p1,p2;
		ar( CEREAL_NVP_("ec",  ec));
		ar( CEREAL_NVP_("what",  what));		
		ar( CEREAL_NVP_("path1",  p1));
		ar( CEREAL_NVP_("path2",  p2));
		err = ::boost::filesystem::filesystem_error(what,p1,p2,ec);
   }
   
         //! Saving for boost::filesystem::file_status 
   template <class Archive> inline
      void save(Archive & ar, ::boost::filesystem::file_status  const & fs)
   {
		ar( CEREAL_NVP_("type",  fs.type()));
		ar( CEREAL_NVP_("permissions",  fs.permissions()));		

   }

   //! Loading for boost::filesystem::file_status 
   template <class Archive> inline
      void load(Archive & ar, ::boost::filesystem::file_status  & fs)
   {
		::boost::filesystem::file_type 	ft;
		::boost::filesystem::perms		permissions;
		ar( CEREAL_NVP_("type",  ft));
		ar( CEREAL_NVP_("permissions",  permissions));		

		fs = ::boost::filesystem::file_status(ft,permissions);
   }
   
            //! Saving for boost::filesystem::directory_entry 
   template <class Archive> inline
      void save(Archive & ar, ::boost::filesystem::directory_entry  const & de)
   {
		ar( CEREAL_NVP_("path",  de.path()));
		ar( CEREAL_NVP_("status",  de.status()));		
		ar( CEREAL_NVP_("symlink_status",  de.symlink_status()));
   }

   //! Loading for boost::filesystem::directory_entry 
   template <class Archive> inline
      void load(Archive & ar, ::boost::filesystem::directory_entry  & de)
   {
		::boost::filesystem::path 	p;
		::boost::filesystem::file_status		status;
		::boost::filesystem::file_status		symlink_status;
		ar( CEREAL_NVP_("path",  p));
		ar( CEREAL_NVP_("status",  status));		
		ar( CEREAL_NVP_("symlink_status",  symlink_status));		

		de = ::boost::filesystem::directory_entry(p,status,symlink_status);
   }
   
   // No attempt made to serialize iterators - it would require serializing the path referenced as well.
   
} // namespace cereal

#endif // CEREAL_TYPES_BOOST_FILESYSTEM_HPP_
