//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014 FURUHASHI Sadayuki and KONDO Takatoshi
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
#ifndef MSGPACK_TYPE_FIXINT_FWD_HPP
#define MSGPACK_TYPE_FIXINT_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

namespace type {


template <typename T>
struct fix_int;

typedef fix_int<uint8_t>  fix_uint8;
typedef fix_int<uint16_t> fix_uint16;
typedef fix_int<uint32_t> fix_uint32;
typedef fix_int<uint64_t> fix_uint64;

typedef fix_int<int8_t>  fix_int8;
typedef fix_int<int16_t> fix_int16;
typedef fix_int<int32_t> fix_int32;
typedef fix_int<int64_t> fix_int64;


}  // namespace type


object const& operator>> (object const& o, type::fix_int8& v);
object const& operator>> (object const& o, type::fix_int16& v);
object const& operator>> (object const& o, type::fix_int32& v);
object const& operator>> (object const& o, type::fix_int64& v);

object const& operator>> (object const& o, type::fix_uint8& v);
object const& operator>> (object const& o, type::fix_uint16& v);
object const& operator>> (object const& o, type::fix_uint32& v);
object const& operator>> (object const& o, type::fix_uint64& v);

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_int8& v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_int16& v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_int32& v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_int64& v);

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_uint8& v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_uint16& v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_uint32& v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const type::fix_uint64& v);

void operator<< (object& o, type::fix_int8 v);
void operator<< (object& o, type::fix_int16 v);
void operator<< (object& o, type::fix_int32 v);
void operator<< (object& o, type::fix_int64 v);

void operator<< (object& o, type::fix_uint8 v);
void operator<< (object& o, type::fix_uint16 v);
void operator<< (object& o, type::fix_uint32 v);
void operator<< (object& o, type::fix_uint64 v);

void operator<< (object::with_zone& o, type::fix_int8 v);
void operator<< (object::with_zone& o, type::fix_int16 v);
void operator<< (object::with_zone& o, type::fix_int32 v);
void operator<< (object::with_zone& o, type::fix_int64 v);

void operator<< (object::with_zone& o, type::fix_uint8 v);
void operator<< (object::with_zone& o, type::fix_uint16 v);
void operator<< (object::with_zone& o, type::fix_uint32 v);
void operator<< (object::with_zone& o, type::fix_uint64 v);

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_FIXINT_FWD_HPP
