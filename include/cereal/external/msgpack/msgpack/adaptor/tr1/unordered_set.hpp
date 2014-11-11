//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
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
#ifndef MSGPACK_TYPE_TR1_UNORDERED_SET_HPP
#define MSGPACK_TYPE_TR1_UNORDERED_SET_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

#if defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#define MSGPACK_HAS_STD_UNOURDERED_SET
#include <unordered_set>
#define MSGPACK_STD_TR1 std

#else   // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if __GNUC__ >= 4

#define MSGPACK_HAS_STD_TR1_UNOURDERED_SET

#include <tr1/unordered_set>
#define MSGPACK_STD_TR1 std::tr1

#endif // __GNUC__ >= 4

#endif  // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if defined(MSGPACK_STD_TR1)

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

template <typename T>
inline object const& operator>> (object const& o, MSGPACK_STD_TR1::unordered_set<T>& v)
{
    if(o.type != type::ARRAY) { throw type_error(); }
    object* p = o.via.array.ptr + o.via.array.size;
    object* const pbegin = o.via.array.ptr;
    MSGPACK_STD_TR1::unordered_set<T> tmp;
    while(p > pbegin) {
        --p;
        tmp.insert(p->as<T>());
    }
    tmp.swap(v);
    return o;
}

template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const MSGPACK_STD_TR1::unordered_set<T>& v)
{
    o.pack_array(v.size());
    for(typename MSGPACK_STD_TR1::unordered_set<T>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
        o.pack(*it);
    }
    return o;
}

template <typename T>
inline void operator<< (object::with_zone& o, const MSGPACK_STD_TR1::unordered_set<T>& v)
{
    o.type = type::ARRAY;
    if(v.empty()) {
        o.via.array.ptr = nullptr;
        o.via.array.size = 0;
    } else {
        object* p = static_cast<object*>(o.zone.allocate_align(sizeof(object)*v.size()));
        object* const pend = p + v.size();
        o.via.array.ptr = p;
        o.via.array.size = v.size();
        typename MSGPACK_STD_TR1::unordered_set<T>::const_iterator it(v.begin());
        do {
            *p = object(*it, o.zone);
            ++p;
            ++it;
        } while(p < pend);
    }
}


template <typename T>
inline object const& operator>> (object const& o, MSGPACK_STD_TR1::unordered_multiset<T>& v)
{
    if(o.type != type::ARRAY) { throw type_error(); }
    object* p = o.via.array.ptr + o.via.array.size;
    object* const pbegin = o.via.array.ptr;
    MSGPACK_STD_TR1::unordered_multiset<T> tmp;
    while(p > pbegin) {
        --p;
        tmp.insert(p->as<T>());
    }
    tmp.swap(v);
    return o;
}

template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const MSGPACK_STD_TR1::unordered_multiset<T>& v)
{
    o.pack_array(v.size());
    for(typename MSGPACK_STD_TR1::unordered_multiset<T>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
        o.pack(*it);
    }
    return o;
}

template <typename T>
inline void operator<< (object::with_zone& o, const MSGPACK_STD_TR1::unordered_multiset<T>& v)
{
    o.type = type::ARRAY;
    if(v.empty()) {
        o.via.array.ptr = nullptr;
        o.via.array.size = 0;
    } else {
        object* p = static_cast<object*>(o.zone.allocate_align(sizeof(object)*v.size()));
        object* const pend = p + v.size();
        o.via.array.ptr = p;
        o.via.array.size = v.size();
        typename MSGPACK_STD_TR1::unordered_multiset<T>::const_iterator it(v.begin());
        do {
            *p = object(*it, o.zone);
            ++p;
            ++it;
        } while(p < pend);
    }
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#undef MSGPACK_STD_TR1

#endif // MSGPACK_STD_TR1

#endif // MSGPACK_TYPE_TR1_UNORDERED_SET_HPP
