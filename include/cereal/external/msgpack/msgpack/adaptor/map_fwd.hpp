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
#ifndef MSGPACK_TYPE_MAP_FWD_HPP
#define MSGPACK_TYPE_MAP_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"
#include <map>
#include <vector>
#include <algorithm>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

namespace type {

template <typename K, typename V>
class assoc_vector;

namespace detail {
    template <typename K, typename V>
    struct pair_first_less;
}  // namespace detail

}  // namespace type


template <typename K, typename V>
object const& operator>> (object const& o, type::assoc_vector<K,V>& v);
template <typename Stream, typename K, typename V>
packer<Stream>& operator<< (packer<Stream>& o, const type::assoc_vector<K,V>& v);
template <typename K, typename V>
void operator<< (object::with_zone& o, const type::assoc_vector<K,V>& v);

template <typename K, typename V>
object const& operator>> (object const& o, std::map<K, V>& v);
template <typename Stream, typename K, typename V>
packer<Stream>& operator<< (packer<Stream>& o, const std::map<K,V>& v);
template <typename K, typename V>
void operator<< (object::with_zone& o, const std::map<K,V>& v);

template <typename K, typename V>
object const& operator>> (object const& o, std::multimap<K, V>& v);
template <typename Stream, typename K, typename V>
packer<Stream>& operator<< (packer<Stream>& o, const std::multimap<K,V>& v);
template <typename K, typename V>
void operator<< (object::with_zone& o, const std::multimap<K,V>& v);

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_MAP_HPP
