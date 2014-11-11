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
#ifndef MSGPACK_CPP11_TUPLE_HPP
#define MSGPACK_CPP11_TUPLE_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

#include <tuple>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

// --- Pack ( from tuple to packer stream ---
template <typename Stream, typename Tuple, std::size_t N>
struct StdTuplePacker {
    static void pack(
        packer<Stream>& o,
        const Tuple& v) {
        StdTuplePacker<Stream, Tuple, N-1>::pack(o, v);
        o.pack(std::get<N-1>(v));
    }
};

template <typename Stream, typename Tuple>
struct StdTuplePacker<Stream, Tuple, 1> {
    static void pack (
        packer<Stream>& o,
        const Tuple& v) {
        o.pack(std::get<0>(v));
    }
};

template <typename Stream, typename Tuple>
struct StdTuplePacker<Stream, Tuple, 0> {
    static void pack (
        packer<Stream>&,
        const Tuple&) {
    }
};


template <typename Stream, typename... Args>
inline const packer<Stream>& operator<< (
    packer<Stream>& o,
    const std::tuple<Args...>& v) {
    o.pack_array(sizeof...(Args));
    StdTuplePacker<Stream, decltype(v), sizeof...(Args)>::pack(o, v);
    return o;
}

// --- Convert from tuple to object ---

template <typename Tuple, std::size_t N>
struct StdTupleConverter {
    static void convert(
        object const& o,
        Tuple& v) {
        StdTupleConverter<Tuple, N-1>::convert(o, v);
        o.via.array.ptr[N-1].convert<typename std::remove_reference<decltype(std::get<N-1>(v))>::type>(std::get<N-1>(v));
    }
};

template <typename Tuple>
struct StdTupleConverter<Tuple, 1> {
    static void convert (
        object const& o,
        Tuple& v) {
        o.via.array.ptr[0].convert<typename std::remove_reference<decltype(std::get<0>(v))>::type>(std::get<0>(v));
    }
};

template <typename Tuple>
struct StdTupleConverter<Tuple, 0> {
    static void convert (
        object const&,
        Tuple&) {
    }
};

template <typename... Args>
inline object const& operator>> (
    object const& o,
    std::tuple<Args...>& v) {
    if(o.type != type::ARRAY) { throw type_error(); }
    if(o.via.array.size < sizeof...(Args)) { throw type_error(); }
    StdTupleConverter<decltype(v), sizeof...(Args)>::convert(o, v);
    return o;
}

// --- Convert from tuple to object with zone ---
template <typename Tuple, std::size_t N>
struct StdTupleToObjectWithZone {
    static void convert(
        object::with_zone& o,
        const Tuple& v) {
        StdTupleToObjectWithZone<Tuple, N-1>::convert(o, v);
        o.via.array.ptr[N-1] = object(std::get<N-1>(v), o.zone);
    }
};

template <typename Tuple>
struct StdTupleToObjectWithZone<Tuple, 1> {
    static void convert (
        object::with_zone& o,
        const Tuple& v) {
        o.via.array.ptr[0] = object(std::get<0>(v), o.zone);
    }
};

template <typename Tuple>
struct StdTupleToObjectWithZone<Tuple, 0> {
    static void convert (
        object::with_zone&,
        const Tuple&) {
    }
};

template <typename... Args>
inline void operator<< (
        object::with_zone& o,
        std::tuple<Args...> const& v) {
    o.type = type::ARRAY;
    o.via.array.ptr = static_cast<object*>(o.zone.allocate_align(sizeof(object)*sizeof...(Args)));
    o.via.array.size = sizeof...(Args);
    StdTupleToObjectWithZone<decltype(v), sizeof...(Args)>::convert(o, v);
}

} // MSGPACK_API_VERSION_NAMESPACE(v1)

} // namespace msgpack

#endif // MSGPACK_CPP11_TUPLE_HPP
