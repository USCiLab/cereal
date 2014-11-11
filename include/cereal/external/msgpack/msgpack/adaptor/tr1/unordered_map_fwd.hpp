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
#ifndef MSGPACK_TYPE_TR1_UNORDERED_MAP_FWD_HPP
#define MSGPACK_TYPE_TR1_UNORDERED_MAP_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

#if defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#define MSGPACK_HAS_STD_UNOURDERED_MAP
#include <unordered_map>
#define MSGPACK_STD_TR1 std

#else   // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if __GNUC__ >= 4

#define MSGPACK_HAS_STD_TR1_UNOURDERED_MAP

#include <tr1/unordered_map>
#define MSGPACK_STD_TR1 std::tr1

#endif // __GNUC__ >= 4

#endif  // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if defined(MSGPACK_STD_TR1)

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

template <typename K, typename V>
object const& operator>> (object const& o, MSGPACK_STD_TR1::unordered_map<K, V>& v);

template <typename Stream, typename K, typename V>
packer<Stream>& operator<< (packer<Stream>& o, const MSGPACK_STD_TR1::unordered_map<K,V>& v);

template <typename K, typename V>
void operator<< (object::with_zone& o, const MSGPACK_STD_TR1::unordered_map<K,V>& v);

template <typename K, typename V>
object const& operator>> (object const& o, MSGPACK_STD_TR1::unordered_multimap<K, V>& v);

template <typename Stream, typename K, typename V>
packer<Stream>& operator<< (packer<Stream>& o, const MSGPACK_STD_TR1::unordered_multimap<K,V>& v);

template <typename K, typename V>
void operator<< (object::with_zone& o, const MSGPACK_STD_TR1::unordered_multimap<K,V>& v);

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#undef MSGPACK_STD_TR1

#endif // MSGPACK_STD_TR1

#endif // MSGPACK_TYPE_TR1_UNORDERED_MAP_FWD_HPP
