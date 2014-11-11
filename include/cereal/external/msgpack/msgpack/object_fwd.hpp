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

#ifndef MSGPACK_OBJECT_FWD_HPP
#define MSGPACK_OBJECT_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/zone.hpp"
#include "msgpack/object.h"

#include <typeinfo>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {


namespace type {
    enum object_type {
        NIL                 = MSGPACK_OBJECT_NIL,
        BOOLEAN             = MSGPACK_OBJECT_BOOLEAN,
        POSITIVE_INTEGER    = MSGPACK_OBJECT_POSITIVE_INTEGER,
        NEGATIVE_INTEGER    = MSGPACK_OBJECT_NEGATIVE_INTEGER,
        DOUBLE              = MSGPACK_OBJECT_DOUBLE,
        STR                 = MSGPACK_OBJECT_STR,
        BIN                 = MSGPACK_OBJECT_BIN,
        ARRAY               = MSGPACK_OBJECT_ARRAY,
        MAP                 = MSGPACK_OBJECT_MAP,
        EXT                 = MSGPACK_OBJECT_EXT
    };
}


struct object;
struct object_kv;

struct object_array {
    uint32_t size;
    object* ptr;
};

struct object_map {
    uint32_t size;
    object_kv* ptr;
};

struct object_str {
    uint32_t size;
    const char* ptr;
};

struct object_bin {
    uint32_t size;
    const char* ptr;
};

struct object_ext {
    int8_t type() const { return ptr[0]; }
    const char* data() const { return &ptr[1]; }
    uint32_t size;
    const char* ptr;
};

struct object {
    union union_type {
        bool boolean;
        uint64_t u64;
        int64_t  i64;
        double   dec;
        object_array array;
        object_map map;
        object_str str;
        object_bin bin;
        object_ext ext;
    };

    type::object_type type;
    union_type via;

    bool is_nil() const { return type == type::NIL; }

    template <typename T>
    T as() const;

    template <typename T>
    void convert(T& v) const;
    template <typename T>
    void convert(T* v) const;

    object();

    object(msgpack_object o);

    template <typename T>
    explicit object(const T& v);

    template <typename T>
    object(const T& v, zone& z);

    // obsolete
    template <typename T>
    object(const T& v, zone* z);

    template <typename T>
    object& operator=(const T& v);

    operator msgpack_object() const;

    struct with_zone;

private:
    struct implicit_type;

public:
    implicit_type convert() const;
};

class type_error : public std::bad_cast { };

struct object_kv {
    object key;
    object val;
};

namespace detail {
template <typename Stream, typename T>
struct packer_serializer;
} // namespace detail

object const& operator>> (object const& o, object& v);

template <typename T>
object const& operator>> (object const& o, T& v);

template <typename T>
void operator<< (object::with_zone& o, const T& v);


template <typename Stream>
class packer;

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const object& v);

template <typename Stream, typename T>
packer<Stream>& operator<< (packer<Stream>& o, const T& v);

template <typename T>
void operator<< (object::with_zone& o, const T& v);

} // MSGPACK_API_VERSION_NAMESPACE(v1)

} // namespace msgpack

#endif // MSGPACK_OBJECT_FWD_HPP
