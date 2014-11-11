//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014 KONDO Takatoshi
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
#ifndef MSGPACK_TYPE_ARRAY_CHAR_HPP
#define MSGPACK_TYPE_ARRAY_CHAR_HPP

#include "msgpack/versioning.hpp"
#include "msgpack_fwd.hpp"
#include <array>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

template <std::size_t N>
inline object const& operator>> (object const& o, std::array<char, N>& v)
{
    switch (o.type) {
    case type::BIN:
        if(o.via.bin.size != N) { throw type_error(); }
        std::memcpy(v.data(), o.via.bin.ptr, o.via.bin.size);
        break;
    case type::STR:
        if(o.via.str.size != N) { throw type_error(); }
        std::memcpy(v.data(), o.via.str.ptr, N);
        break;
    default:
        throw type_error();
        break;
    }
    return o;
}

template <typename Stream, std::size_t N>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::array<char, N>& v)
{
    o.pack_bin(v.size());
    o.pack_bin_body(v.data(), v.size());

    return o;
}

template <std::size_t N>
inline void operator<< (object& o, const std::array<char, N>& v)
{
    o.type = type::BIN;
    o.via.bin.ptr = v.data();
    o.via.bin.size = static_cast<uint32_t>(v.size());
}

template <std::size_t N>
inline void operator<< (object::with_zone& o, const std::array<char, N>& v)
{
    o.type = type::BIN;
    char* ptr = static_cast<char*>(o.zone.allocate_align(v.size()));
    o.via.bin.ptr = ptr;
    o.via.bin.size = static_cast<uint32_t>(v.size());
    std::memcpy(ptr, v.data(), v.size());
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_ARRAY_CHAR_HPP
