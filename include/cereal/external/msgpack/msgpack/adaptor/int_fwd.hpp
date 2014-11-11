//
// MessagePack for C++ forward declaration
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
#ifndef MSGPACK_TYPE_INT_FWD_HPP
#define MSGPACK_TYPE_INT_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1){

object const& operator>> (object const& o, char& v);

object const& operator>> (object const& o, signed char& v);
object const& operator>> (object const& o, signed short& v);
object const& operator>> (object const& o, signed int& v);
object const& operator>> (object const& o, signed long& v);
object const& operator>> (object const& o, signed long long& v);

object const& operator>> (object const& o, unsigned char& v);
object const& operator>> (object const& o, unsigned short& v);
object const& operator>> (object const& o, unsigned int& v);
object const& operator>> (object const& o, unsigned long& v);
object const& operator>> (object const& o, unsigned long long& v);

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, char v);

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, signed char v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, signed short v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, signed int v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, signed long v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, signed long long v);

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, unsigned char v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, unsigned short v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, unsigned int v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, unsigned long v);
template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, unsigned long long v);


void operator<< (object& o, char v);

void operator<< (object& o, signed char v);
void operator<< (object& o, signed short v);
void operator<< (object& o, signed int v);
void operator<< (object& o, signed long v);
void operator<< (object& o, signed long long v);

void operator<< (object& o, unsigned char v);
void operator<< (object& o, unsigned short v);
void operator<< (object& o, unsigned int v);
void operator<< (object& o, unsigned long v);
void operator<< (object& o, unsigned long long v);

void operator<< (object::with_zone& o, char v);

void operator<< (object::with_zone& o, signed char v);
void operator<< (object::with_zone& o, signed short v);
void operator<< (object::with_zone& o, signed int v);
void operator<< (object::with_zone& o, signed long v);
void operator<< (object::with_zone& o, const signed long long& v);

void operator<< (object::with_zone& o, unsigned char v);
void operator<< (object::with_zone& o, unsigned short v);
void operator<< (object::with_zone& o, unsigned int v);
void operator<< (object::with_zone& o, unsigned long v);
void operator<< (object::with_zone& o, const unsigned long long& v);

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_INT_FWD_HPP
