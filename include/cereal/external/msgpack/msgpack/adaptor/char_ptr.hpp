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
#ifndef MSGPACK_TYPE_CHAR_PTR_HPP
#define MSGPACK_TYPE_CHAR_PTR_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"
#include <cstring>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const char* v)
{
    std::size_t size = std::strlen(v);
    o.pack_str(size);
    o.pack_str_body(v, size);
    return o;
}

inline void operator<< (object::with_zone& o, const char* v)
{
    std::size_t size = std::strlen(v);
    o.type = type::STR;
    char* ptr = static_cast<char*>(o.zone.allocate_align(size));
    o.via.str.ptr = ptr;
    o.via.str.size = static_cast<uint32_t>(size);
    memcpy(ptr, v, size);
}

inline void operator<< (object& o, const char* v)
{
    std::size_t size = std::strlen(v);
    o.type = type::STR;
    o.via.str.ptr = v;
    o.via.str.size = static_cast<uint32_t>(size);
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_CHAR_PTR_HPP
