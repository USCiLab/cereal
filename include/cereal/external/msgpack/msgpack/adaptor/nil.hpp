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
#ifndef MSGPACK_TYPE_NIL_HPP
#define MSGPACK_TYPE_NIL_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

namespace type {

struct nil { };

}  // namespace type


inline object const& operator>> (object const& o, type::nil&)
{
    if(o.type != type::NIL) { throw type_error(); }
    return o;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const type::nil&)
{
    o.pack_nil();
    return o;
}

inline void operator<< (object& o, type::nil)
{
    o.type = type::NIL;
}

inline void operator<< (object::with_zone& o, type::nil v)
    { static_cast<object&>(o) << v; }


template <>
inline void object::as<void>() const
{
    msgpack::type::nil v;
    convert(v);
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif // MSGPACK_TYPE_NIL_HPP
