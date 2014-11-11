//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2014 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_TYPE_NIL_FWD_HPP
#define MSGPACK_TYPE_NIL_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

namespace type {

struct nil;

}  // namespace type


object const& operator>> (object const& o, type::nil&);

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::nil&);

void operator<< (object& o, type::nil);

void operator<< (object::with_zone& o, type::nil v);

template <>
inline void object::as<void>() const;

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_NIL_FWD_HPP
