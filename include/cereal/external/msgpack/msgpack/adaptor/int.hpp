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
#ifndef MSGPACK_TYPE_INT_HPP
#define MSGPACK_TYPE_INT_HPP

#include "msgpack/versioning.hpp"
#include "msgpack_fwd.hpp"
#include <limits>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1){

namespace type {
namespace detail {
    template <typename T, bool Signed>
    struct convert_integer_sign;

    template <typename T>
    struct convert_integer_sign<T, true> {
        static inline T convert(object const& o) {
            if(o.type == type::POSITIVE_INTEGER) {
                if(o.via.u64 > static_cast<uint64_t>(std::numeric_limits<T>::max()))
                    { throw type_error(); }
                return static_cast<T>(o.via.u64);
            } else if(o.type == type::NEGATIVE_INTEGER) {
                if(o.via.i64 < static_cast<int64_t>(std::numeric_limits<T>::min()))
                    { throw type_error(); }
                return static_cast<T>(o.via.i64);
            }
            throw type_error();
        }
    };

    template <typename T>
    struct convert_integer_sign<T, false> {
        static inline T convert(object const& o) {
            if(o.type == type::POSITIVE_INTEGER) {
                if(o.via.u64 > static_cast<uint64_t>(std::numeric_limits<T>::max()))
                    { throw type_error(); }
                return static_cast<T>(o.via.u64);
            }
            throw type_error();
        }
    };

    template <typename T>
    struct is_signed {
        static const bool value = std::numeric_limits<T>::is_signed;
    };

    template <typename T>
    static inline T convert_integer(object const& o)
    {
        return detail::convert_integer_sign<T, is_signed<T>::value>::convert(o);
    }

    template <bool Signed>
    struct object_char_sign;

    template <>
    struct object_char_sign<true> {
        static inline void make(object& o, char v) {
            if (v < 0) {
                o.type = type::NEGATIVE_INTEGER;
                o.via.i64 = v;
            }
            else {
                o.type = type::POSITIVE_INTEGER;
                o.via.u64 = v;
            }
        }
    };

    template <>
    struct object_char_sign<false> {
        static inline void make(object& o, char v) {
            o.type = type::POSITIVE_INTEGER, o.via.u64 = v;
        }
    };

    static inline void object_char(object& o, char v) {
        return object_char_sign<is_signed<char>::value>::make(o, v);
    }

}  // namespace detail
}  // namespace type

inline object const& operator>> (object const& o, char& v)
    { v = type::detail::convert_integer<char>(o); return o; }


inline object const& operator>> (object const& o, signed char& v)
    { v = type::detail::convert_integer<signed char>(o); return o; }

inline object const& operator>> (object const& o, signed short& v)
    { v = type::detail::convert_integer<signed short>(o); return o; }

inline object const& operator>> (object const& o, signed int& v)
    { v = type::detail::convert_integer<signed int>(o); return o; }

inline object const& operator>> (object const& o, signed long& v)
    { v = type::detail::convert_integer<signed long>(o); return o; }

inline object const& operator>> (object const& o, signed long long& v)
    { v = type::detail::convert_integer<signed long long>(o); return o; }


inline object const& operator>> (object const& o, unsigned char& v)
    { v = type::detail::convert_integer<unsigned char>(o); return o; }

inline object const& operator>> (object const& o, unsigned short& v)
    { v = type::detail::convert_integer<unsigned short>(o); return o; }

inline object const& operator>> (object const& o, unsigned int& v)
    { v = type::detail::convert_integer<unsigned int>(o); return o; }

inline object const& operator>> (object const& o, unsigned long& v)
    { v = type::detail::convert_integer<unsigned long>(o); return o; }

inline object const& operator>> (object const& o, unsigned long long& v)
    { v = type::detail::convert_integer<unsigned long long>(o); return o; }


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, char v)
    { o.pack_char(v); return o; }


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed char v)
    { o.pack_signed_char(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed short v)
    { o.pack_short(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed int v)
    { o.pack_int(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed long v)
    { o.pack_long(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed long long v)
    { o.pack_long_long(v); return o; }


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned char v)
    { o.pack_unsigned_char(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned short v)
    { o.pack_unsigned_short(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned int v)
    { o.pack_unsigned_int(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned long v)
    { o.pack_unsigned_long(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned long long v)
    { o.pack_unsigned_long_long(v); return o; }


inline void operator<< (object& o, char v)
    { type::detail::object_char(o, v); }


inline void operator<< (object& o, signed char v)
{
    if (v < 0) {
        o.type = type::NEGATIVE_INTEGER;
        o.via.i64 = v;
    }
    else {
        o.type = type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
}

inline void operator<< (object& o, signed short v)
{
    if (v < 0) {
        o.type = type::NEGATIVE_INTEGER;
        o.via.i64 = v;
    }
    else {
        o.type = type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
}

inline void operator<< (object& o, signed int v)
{
    if (v < 0) {
        o.type = type::NEGATIVE_INTEGER;
        o.via.i64 = v;
    }
    else {
        o.type = type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
}

inline void operator<< (object& o, signed long v)
{
    if (v < 0) {
        o.type = type::NEGATIVE_INTEGER;
        o.via.i64 = v;
    }
    else {
        o.type = type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
}

inline void operator<< (object& o, signed long long v)
{
    if (v < 0) {
        o.type = type::NEGATIVE_INTEGER;
        o.via.i64 = v;
    }
    else{
        o.type = type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
}

inline void operator<< (object& o, unsigned char v)
    { o.type = type::POSITIVE_INTEGER, o.via.u64 = v; }

inline void operator<< (object& o, unsigned short v)
    { o.type = type::POSITIVE_INTEGER, o.via.u64 = v; }

inline void operator<< (object& o, unsigned int v)
    { o.type = type::POSITIVE_INTEGER, o.via.u64 = v; }

inline void operator<< (object& o, unsigned long v)
    { o.type = type::POSITIVE_INTEGER, o.via.u64 = v; }

inline void operator<< (object& o, unsigned long long v)
    { o.type = type::POSITIVE_INTEGER, o.via.u64 = v; }



inline void operator<< (object::with_zone& o, char v)
    { static_cast<object&>(o) << v; }


inline void operator<< (object::with_zone& o, signed char v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, signed short v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, signed int v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, signed long v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, const signed long long& v)
    { static_cast<object&>(o) << v; }


inline void operator<< (object::with_zone& o, unsigned char v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, unsigned short v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, unsigned int v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, unsigned long v)
    { static_cast<object&>(o) << v; }

inline void operator<< (object::with_zone& o, const unsigned long long& v)
    { static_cast<object&>(o) << v; }


}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif /* msgpack/type/int.hpp */
