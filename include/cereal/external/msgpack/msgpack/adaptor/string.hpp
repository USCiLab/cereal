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
#ifndef MSGPACK_TYPE_STRING_HPP
#define MSGPACK_TYPE_STRING_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"
#include <string>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

inline object const& operator>> (object const& o, std::string& v)
{
    switch (o.type) {
    case type::BIN:
        v.assign(o.via.bin.ptr, o.via.bin.size);
        break;
    case type::STR:
        v.assign(o.via.str.ptr, o.via.str.size);
        break;
    default:
        throw type_error();
        break;
    }
    return o;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::string& v)
{
    o.pack_str(v.size());
    o.pack_str_body(v.data(), v.size());
    return o;
}

inline void operator<< (object::with_zone& o, const std::string& v)
{
    o.type = type::STR;
    char* ptr = static_cast<char*>(o.zone.allocate_align(v.size()));
    o.via.str.ptr = ptr;
    o.via.str.size = static_cast<uint32_t>(v.size());
    memcpy(ptr, v.data(), v.size());
}

inline void operator<< (object& o, const std::string& v)
{
    o.type = type::STR;
    o.via.str.ptr = v.data();
    o.via.str.size = static_cast<uint32_t>(v.size());
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_STRING_HPP
