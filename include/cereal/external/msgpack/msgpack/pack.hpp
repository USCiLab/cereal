//
// MessagePack for C++ serializing routine
//
// Copyright (C) 2008-2013 FURUHASHI Sadayuki and KONDO Takatoshi
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
#ifndef MSGPACK_PACK_HPP
#define MSGPACK_PACK_HPP

#include "msgpack/versioning.hpp"

#include <stdexcept>
#include <limits>
#include <cstring>
#include <climits>

#include "sysdep.h"

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

template <typename Stream>
class packer {
public:
    packer(Stream* s);
    packer(Stream& s);

public:
    template <typename T>
    packer<Stream>& pack(const T& v);

    packer<Stream>& pack_uint8(uint8_t d);
    packer<Stream>& pack_uint16(uint16_t d);
    packer<Stream>& pack_uint32(uint32_t d);
    packer<Stream>& pack_uint64(uint64_t d);
    packer<Stream>& pack_int8(int8_t d);
    packer<Stream>& pack_int16(int16_t d);
    packer<Stream>& pack_int32(int32_t d);
    packer<Stream>& pack_int64(int64_t d);

    packer<Stream>& pack_fix_uint8(uint8_t d);
    packer<Stream>& pack_fix_uint16(uint16_t d);
    packer<Stream>& pack_fix_uint32(uint32_t d);
    packer<Stream>& pack_fix_uint64(uint64_t d);
    packer<Stream>& pack_fix_int8(int8_t d);
    packer<Stream>& pack_fix_int16(int16_t d);
    packer<Stream>& pack_fix_int32(int32_t d);
    packer<Stream>& pack_fix_int64(int64_t d);

    packer<Stream>& pack_char(char d);
    packer<Stream>& pack_signed_char(signed char d);
    packer<Stream>& pack_short(short d);
    packer<Stream>& pack_int(int d);
    packer<Stream>& pack_long(long d);
    packer<Stream>& pack_long_long(long long d);
    packer<Stream>& pack_unsigned_char(unsigned char d);
    packer<Stream>& pack_unsigned_short(unsigned short d);
    packer<Stream>& pack_unsigned_int(unsigned int d);
    packer<Stream>& pack_unsigned_long(unsigned long d);
    packer<Stream>& pack_unsigned_long_long(unsigned long long d);

    packer<Stream>& pack_float(float d);
    packer<Stream>& pack_double(double d);

    packer<Stream>& pack_nil();
    packer<Stream>& pack_true();
    packer<Stream>& pack_false();

    packer<Stream>& pack_array(size_t n);

    packer<Stream>& pack_map(size_t n);

    packer<Stream>& pack_str(size_t l);
    packer<Stream>& pack_str_body(const char* b, size_t l);

    packer<Stream>& pack_bin(size_t l);
    packer<Stream>& pack_bin_body(const char* b, size_t l);

    packer<Stream>& pack_ext(size_t l, int8_t type);
    packer<Stream>& pack_ext_body(const char* b, size_t l);

private:
    template <typename T>
    void pack_imp_uint8(T d);
    template <typename T>
    void pack_imp_uint16(T d);
    template <typename T>
    void pack_imp_uint32(T d);
    template <typename T>
    void pack_imp_uint64(T d);
    template <typename T>
    void pack_imp_int8(T d);
    template <typename T>
    void pack_imp_int16(T d);
    template <typename T>
    void pack_imp_int32(T d);
    template <typename T>
    void pack_imp_int64(T d);

    void append_buffer(const char* buf, size_t len)
        { m_stream.write(buf, len); }

private:
    Stream& m_stream;

private:
    packer();
};


template <typename Stream, typename T>
inline void pack(Stream* s, const T& v)
{
    packer<Stream>(*s).pack(v);
}

template <typename Stream, typename T>
inline void pack(Stream& s, const T& v)
{
    packer<Stream>(s).pack(v);
}

#if defined(__LITTLE_ENDIAN__)
template <typename T>
inline char take8_8(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[0]);
}
template <typename T>
inline char take8_16(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[0]);
}
template <typename T>
inline char take8_32(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[0]);
}
template <typename T>
inline char take8_64(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[0]);
}

#elif defined(__BIG_ENDIAN__)

template <typename T>
inline char take8_8(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[0]);
}
template <typename T>
inline char take8_16(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[1]);
}
template <typename T>
inline char take8_32(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[3]);
}
template <typename T>
inline char take8_64(T d) {
    return static_cast<char>(reinterpret_cast<uint8_t*>(&d)[7]);
}

#endif

template <typename Stream>
inline packer<Stream>::packer(Stream* s) : m_stream(*s) { }

template <typename Stream>
inline packer<Stream>::packer(Stream& s) : m_stream(s) { }


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_uint8(uint8_t d)
{ pack_imp_uint8(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_uint16(uint16_t d)
{ pack_imp_uint16(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_uint32(uint32_t d)
{ pack_imp_uint32(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_uint64(uint64_t d)
{ pack_imp_uint64(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_int8(int8_t d)
{ pack_imp_int8(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_int16(int16_t d)
{ pack_imp_int16(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_int32(int32_t d)
{ pack_imp_int32(d); return *this; }

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_int64(int64_t d)
{ pack_imp_int64(d); return *this;}


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_uint8(uint8_t d)
{
    char buf[2] = {static_cast<char>(0xcc), take8_8(d)};
    append_buffer(buf, 2);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_uint16(uint16_t d)
{
    char buf[3];
    buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], d);
    append_buffer(buf, 3);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_uint32(uint32_t d)
{
    char buf[5];
    buf[0] = static_cast<char>(0xce); _msgpack_store32(&buf[1], d);
    append_buffer(buf, 5);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_uint64(uint64_t d)
{
    char buf[9];
    buf[0] = static_cast<char>(0xcf); _msgpack_store64(&buf[1], d);
    append_buffer(buf, 9);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_int8(int8_t d)
{
    char buf[2] = {static_cast<char>(0xd0), take8_8(d)};
    append_buffer(buf, 2);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_int16(int16_t d)
{
    char buf[3];
    buf[0] = static_cast<char>(0xd1); _msgpack_store16(&buf[1], d);
    append_buffer(buf, 3);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_int32(int32_t d)
{
    char buf[5];
    buf[0] = static_cast<char>(0xd2); _msgpack_store32(&buf[1], d);
    append_buffer(buf, 5);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_fix_int64(int64_t d)
{
    char buf[9];
    buf[0] = static_cast<char>(0xd3); _msgpack_store64(&buf[1], d);
    append_buffer(buf, 9);
    return *this;
}


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_char(char d)
{
#if defined(CHAR_MIN)
#if CHAR_MIN < 0
    pack_imp_int8(d);
#else
    pack_imp_uint8(d);
#endif
#else
#error CHAR_MIN is not defined
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_signed_char(signed char d)
{
    pack_imp_int8(d);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_short(short d)
{
#if defined(SIZEOF_SHORT)
#if SIZEOF_SHORT == 2
    pack_imp_int16(d);
#elif SIZEOF_SHORT == 4
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#elif defined(SHRT_MAX)
#if SHRT_MAX == 0x7fff
    pack_imp_int16(d);
#elif SHRT_MAX == 0x7fffffff
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#else
    if(sizeof(short) == 2) {
        pack_imp_int16(d);
    } else if(sizeof(short) == 4) {
        pack_imp_int32(d);
    } else {
        pack_imp_int64(d);
    }
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_int(int d)
{
#if defined(SIZEOF_INT)
#if SIZEOF_INT == 2
    pack_imp_int16(d);
#elif SIZEOF_INT == 4
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#elif defined(INT_MAX)
#if INT_MAX == 0x7fff
    pack_imp_int16(d);
#elif INT_MAX == 0x7fffffff
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#else
    if(sizeof(int) == 2) {
        pack_imp_int16(d);
    } else if(sizeof(int) == 4) {
        pack_imp_int32(d);
    } else {
        pack_imp_int64(d);
    }
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_long(long d)
{
#if defined(SIZEOF_LONG)
#if SIZEOF_LONG == 2
    pack_imp_int16(d);
#elif SIZEOF_LONG == 4
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#elif defined(LONG_MAX)
#if LONG_MAX == 0x7fffL
    pack_imp_int16(d);
#elif LONG_MAX == 0x7fffffffL
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#else
    if(sizeof(long) == 2) {
        pack_imp_int16(d);
    } else if(sizeof(long) == 4) {
        pack_imp_int32(d);
    } else {
        pack_imp_int64(d);
    }
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_long_long(long long d)
{
#if defined(SIZEOF_LONG_LONG)
#if SIZEOF_LONG_LONG == 2
    pack_imp_int16(d);
#elif SIZEOF_LONG_LONG == 4
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#elif defined(LLONG_MAX)
#if LLONG_MAX == 0x7fffL
    pack_imp_int16(d);
#elif LLONG_MAX == 0x7fffffffL
    pack_imp_int32(d);
#else
    pack_imp_int64(d);
#endif

#else
    if(sizeof(long long) == 2) {
        pack_imp_int16(d);
    } else if(sizeof(long long) == 4) {
        pack_imp_int32(d);
    } else {
        pack_imp_int64(d);
    }
#endif
    return *this;
}


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_unsigned_char(unsigned char d)
{
    pack_imp_uint8(d);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_unsigned_short(unsigned short d)
{
#if defined(SIZEOF_SHORT)
#if SIZEOF_SHORT == 2
    pack_imp_uint16(d);
#elif SIZEOF_SHORT == 4
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#elif defined(USHRT_MAX)
#if USHRT_MAX == 0xffffU
    pack_imp_uint16(d);
#elif USHRT_MAX == 0xffffffffU
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#else
    if(sizeof(unsigned short) == 2) {
        pack_imp_uint16(d);
    } else if(sizeof(unsigned short) == 4) {
        pack_imp_uint32(d);
    } else {
        pack_imp_uint64(d);
    }
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_unsigned_int(unsigned int d)
{
#if defined(SIZEOF_INT)
#if SIZEOF_INT == 2
    pack_imp_uint16(d);
#elif SIZEOF_INT == 4
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#elif defined(UINT_MAX)
#if UINT_MAX == 0xffffU
    pack_imp_uint16(d);
#elif UINT_MAX == 0xffffffffU
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#else
    if(sizeof(unsigned int) == 2) {
        pack_imp_uint16(d);
    } else if(sizeof(unsigned int) == 4) {
        pack_imp_uint32(d);
    } else {
        pack_imp_uint64(d);
    }
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_unsigned_long(unsigned long d)
{
#if defined(SIZEOF_LONG)
#if SIZEOF_LONG == 2
    pack_imp_uint16(d);
#elif SIZEOF_LONG == 4
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#elif defined(ULONG_MAX)
#if ULONG_MAX == 0xffffUL
    pack_imp_uint16(d);
#elif ULONG_MAX == 0xffffffffUL
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#else
    if(sizeof(unsigned long) == 2) {
        pack_imp_uint16(d);
    } else if(sizeof(unsigned long) == 4) {
        pack_imp_uint32(d);
    } else {
        pack_imp_uint64(d);
    }
#endif
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_unsigned_long_long(unsigned long long d)
{
#if defined(SIZEOF_LONG_LONG)
#if SIZEOF_LONG_LONG == 2
    pack_imp_uint16(d);
#elif SIZEOF_LONG_LONG == 4
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#elif defined(ULLONG_MAX)
#if ULLONG_MAX == 0xffffUL
    pack_imp_uint16(d);
#elif ULLONG_MAX == 0xffffffffUL
    pack_imp_uint32(d);
#else
    pack_imp_uint64(d);
#endif

#else
    if(sizeof(unsigned long long) == 2) {
        pack_imp_uint16(d);
    } else if(sizeof(unsigned long long) == 4) {
        pack_imp_uint32(d);
    } else {
        pack_imp_uint64(d);
    }
#endif
    return *this;
}


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_float(float d)
{
    union { float f; uint32_t i; } mem;
    mem.f = d;
    char buf[5];
    buf[0] = static_cast<char>(0xca); _msgpack_store32(&buf[1], mem.i);
    append_buffer(buf, 5);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_double(double d)
{
    union { double f; uint64_t i; } mem;
    mem.f = d;
    char buf[9];
    buf[0] = static_cast<char>(0xcb);
#if defined(__arm__) && !(__ARM_EABI__) // arm-oabi
    // https://github.com/msgpack/msgpack-perl/pull/1
    mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
    _msgpack_store64(&buf[1], mem.i);
    append_buffer(buf, 9);
    return *this;
}


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_nil()
{
    const char d = static_cast<char>(0xc0);
    append_buffer(&d, 1);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_true()
{
    const char d = static_cast<char>(0xc3);
    append_buffer(&d, 1);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_false()
{
    const char d = static_cast<char>(0xc2);
    append_buffer(&d, 1);
    return *this;
}


template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_array(size_t n)
{
    if(n < 16) {
        char d = static_cast<char>(0x90 | n);
        append_buffer(&d, 1);
    } else if(n < 65536) {
        char buf[3];
        buf[0] = static_cast<char>(0xdc); _msgpack_store16(&buf[1], static_cast<uint16_t>(n));
        append_buffer(buf, 3);
    } else {
        char buf[5];
        buf[0] = static_cast<char>(0xdd); _msgpack_store32(&buf[1], static_cast<uint32_t>(n));
        append_buffer(buf, 5);
    }
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_map(size_t n)
{
    if(n < 16) {
        unsigned char d = 0x80 | n;
        char buf = take8_8(d);
        append_buffer(&buf, 1);
    } else if(n < 65536) {
        char buf[3];
        buf[0] = static_cast<char>(0xde); _msgpack_store16(&buf[1], static_cast<uint16_t>(n));
        append_buffer(buf, 3);
    } else {
        char buf[5];
        buf[0] = static_cast<char>(0xdf); _msgpack_store32(&buf[1], static_cast<uint32_t>(n));
        append_buffer(buf, 5);
    }
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_str(size_t l)
{
    if(l < 32) {
        unsigned char d = 0xa0 | static_cast<uint8_t>(l);
        char buf = take8_8(d);
        append_buffer(&buf, 1);
    } else if(l < 256) {
        char buf[2];
        buf[0] = static_cast<char>(0xd9); buf[1] = static_cast<uint8_t>(l);
        append_buffer(buf, 2);
    } else if(l < 65536) {
        char buf[3];
        buf[0] = static_cast<char>(0xda); _msgpack_store16(&buf[1], static_cast<uint16_t>(l));
        append_buffer(buf, 3);
    } else {
        char buf[5];
        buf[0] = static_cast<char>(0xdb); _msgpack_store32(&buf[1], static_cast<uint32_t>(l));
        append_buffer(buf, 5);
    }
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_str_body(const char* b, size_t l)
{
    append_buffer(b, l);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_bin(size_t l)
{
    if(l < 256) {
        char buf[2];
        buf[0] = static_cast<char>(0xc4); buf[1] = static_cast<uint8_t>(l);
        append_buffer(buf, 2);
    } else if(l < 65536) {
        char buf[3];
        buf[0] = static_cast<char>(0xc5); _msgpack_store16(&buf[1], static_cast<uint16_t>(l));
        append_buffer(buf, 3);
    } else {
        char buf[5];
        buf[0] = static_cast<char>(0xc6); _msgpack_store32(&buf[1], static_cast<uint32_t>(l));
        append_buffer(buf, 5);
    }
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_bin_body(const char* b, size_t l)
{
    append_buffer(b, l);
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_ext(size_t l, int8_t type)
{
    switch(l) {
    case 1: {
        char buf[2];
        buf[0] = static_cast<char>(0xd4);
        buf[1] = static_cast<char>(type);
        append_buffer(buf, 2);
    } break;
    case 2: {
        char buf[2];
        buf[0] = static_cast<char>(0xd5);
        buf[1] = static_cast<char>(type);
        append_buffer(buf, 2);
    } break;
    case 4: {
        char buf[2];
        buf[0] = static_cast<char>(0xd6);
        buf[1] = static_cast<char>(type);
        append_buffer(buf, 2);
    } break;
    case 8: {
        char buf[2];
        buf[0] = static_cast<char>(0xd7);
        buf[1] = static_cast<char>(type);
        append_buffer(buf, 2);
    } break;
    case 16: {
        char buf[2];
        buf[0] = static_cast<char>(0xd8);
        buf[1] = static_cast<char>(type);
        append_buffer(buf, 2);
    } break;
    default:
        if(l < 256) {
            char buf[3];
            buf[0] = static_cast<char>(0xc7);
            buf[1] = static_cast<char>(l);
            buf[2] = static_cast<char>(type);
            append_buffer(buf, 3);
        } else if(l < 65536) {
            char buf[4];
            buf[0] = static_cast<char>(0xc8);
            _msgpack_store16(&buf[1], static_cast<uint16_t>(l));
            buf[3] = static_cast<char>(type);
            append_buffer(buf, 4);
        } else {
            char buf[6];
            buf[0] = static_cast<char>(0xc9);
            _msgpack_store32(&buf[1], static_cast<uint32_t>(l));
            buf[5] = static_cast<char>(type);
            append_buffer(buf, 6);
        }
        break;
    }
    return *this;
}

template <typename Stream>
inline packer<Stream>& packer<Stream>::pack_ext_body(const char* b, size_t l)
{
    append_buffer(b, l);
    return *this;
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_uint8(T d)
{
    if(d < (1<<7)) {
        /* fixnum */
        char buf = take8_8(d);
        append_buffer(&buf, 1);
    } else {
        /* unsigned 8 */
        char buf[2] = {static_cast<char>(0xcc), take8_8(d)};
        append_buffer(buf, 2);
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_uint16(T d)
{
    if(d < (1<<7)) {
        /* fixnum */
        char buf = take8_16(d);
        append_buffer(&buf, 1);
    } else if(d < (1<<8)) {
        /* unsigned 8 */
        char buf[2] = {static_cast<char>(0xcc), take8_16(d)};
        append_buffer(buf, 2);
    } else {
        /* unsigned 16 */
        char buf[3];
        buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], static_cast<uint16_t>(d));
        append_buffer(buf, 3);
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_uint32(T d)
{
    if(d < (1<<8)) {
        if(d < (1<<7)) {
            /* fixnum */
            char buf = take8_32(d);
            append_buffer(&buf, 1);
        } else {
            /* unsigned 8 */
            char buf[2] = {static_cast<char>(0xcc), take8_32(d)};
            append_buffer(buf, 2);
        }
    } else {
        if(d < (1<<16)) {
            /* unsigned 16 */
            char buf[3];
            buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], static_cast<uint16_t>(d));
            append_buffer(buf, 3);
        } else {
            /* unsigned 32 */
            char buf[5];
            buf[0] = static_cast<char>(0xce); _msgpack_store32(&buf[1], static_cast<uint32_t>(d));
            append_buffer(buf, 5);
        }
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_uint64(T d)
{
    if(d < (1ULL<<8)) {
        if(d < (1ULL<<7)) {
            /* fixnum */
            char buf = take8_64(d);
            append_buffer(&buf, 1);
        } else {
            /* unsigned 8 */
            char buf[2] = {static_cast<char>(0xcc), take8_64(d)};
            append_buffer(buf, 2);
        }
    } else {
        if(d < (1ULL<<16)) {
            /* unsigned 16 */
            char buf[3];
            buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], static_cast<uint16_t>(d));
            append_buffer(buf, 3);
        } else if(d < (1ULL<<32)) {
            /* unsigned 32 */
            char buf[5];
            buf[0] = static_cast<char>(0xce); _msgpack_store32(&buf[1], static_cast<uint32_t>(d));
            append_buffer(buf, 5);
        } else {
            /* unsigned 64 */
            char buf[9];
            buf[0] = static_cast<char>(0xcf); _msgpack_store64(&buf[1], d);
            append_buffer(buf, 9);
        }
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_int8(T d)
{
    if(d < -(1<<5)) {
        /* signed 8 */
        char buf[2] = {static_cast<char>(0xd0), take8_8(d)};
        append_buffer(buf, 2);
    } else {
        /* fixnum */
        char buf = take8_8(d);
        append_buffer(&buf, 1);
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_int16(T d)
{
    if(d < -(1<<5)) {
        if(d < -(1<<7)) {
            /* signed 16 */
            char buf[3];
            buf[0] = static_cast<char>(0xd1); _msgpack_store16(&buf[1], static_cast<int16_t>(d));
            append_buffer(buf, 3);
        } else {
            /* signed 8 */
            char buf[2] = {static_cast<char>(0xd0), take8_16(d)};
            append_buffer(buf, 2);
        }
    } else if(d < (1<<7)) {
        /* fixnum */
        char buf = take8_16(d);
        append_buffer(&buf, 1);
    } else {
        if(d < (1<<8)) {
            /* unsigned 8 */
            char buf[2] = {static_cast<char>(0xcc), take8_16(d)};
            append_buffer(buf, 2);
        } else {
            /* unsigned 16 */
            char buf[3];
            buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], static_cast<uint16_t>(d));
            append_buffer(buf, 3);
        }
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_int32(T d)
{
    if(d < -(1<<5)) {
        if(d < -(1<<15)) {
            /* signed 32 */
            char buf[5];
            buf[0] = static_cast<char>(0xd2); _msgpack_store32(&buf[1], static_cast<int32_t>(d));
            append_buffer(buf, 5);
        } else if(d < -(1<<7)) {
            /* signed 16 */
            char buf[3];
            buf[0] = static_cast<char>(0xd1); _msgpack_store16(&buf[1], static_cast<int16_t>(d));
            append_buffer(buf, 3);
        } else {
            /* signed 8 */
            char buf[2] = { static_cast<char>(0xd0), take8_32(d)};
            append_buffer(buf, 2);
        }
    } else if(d < (1<<7)) {
        /* fixnum */
        char buf = take8_32(d);
        append_buffer(&buf, 1);
    } else {
        if(d < (1<<8)) {
            /* unsigned 8 */
            char buf[2] = { static_cast<char>(0xcc), take8_32(d)};
            append_buffer(buf, 2);
        } else if(d < (1<<16)) {
            /* unsigned 16 */
            char buf[3];
            buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], static_cast<uint16_t>(d));
            append_buffer(buf, 3);
        } else {
            /* unsigned 32 */
            char buf[5];
            buf[0] = static_cast<char>(0xce); _msgpack_store32(&buf[1], static_cast<uint32_t>(d));
            append_buffer(buf, 5);
        }
    }
}

template <typename Stream>
template <typename T>
inline void packer<Stream>::pack_imp_int64(T d)
{
    if(d < -(1LL<<5)) {
        if(d < -(1LL<<15)) {
            if(d < -(1LL<<31)) {
                /* signed 64 */
                char buf[9];
                buf[0] = static_cast<char>(0xd3); _msgpack_store64(&buf[1], d);
                append_buffer(buf, 9);
            } else {
                /* signed 32 */
                char buf[5];
                buf[0] = static_cast<char>(0xd2); _msgpack_store32(&buf[1], static_cast<int32_t>(d));
                append_buffer(buf, 5);
            }
        } else {
            if(d < -(1<<7)) {
                /* signed 16 */
                char buf[3];
                buf[0] = static_cast<char>(0xd1); _msgpack_store16(&buf[1], static_cast<int16_t>(d));
                append_buffer(buf, 3);
            } else {
                /* signed 8 */
                char buf[2] = {static_cast<char>(0xd0), take8_64(d)};
                append_buffer(buf, 2);
            }
        }
    } else if(d < (1<<7)) {
        /* fixnum */
        char buf = take8_64(d);
        append_buffer(&buf, 1);
    } else {
        if(d < (1LL<<16)) {
            if(d < (1<<8)) {
                /* unsigned 8 */
                char buf[2] = {static_cast<char>(0xcc), take8_64(d)};
                append_buffer(buf, 2);
            } else {
                /* unsigned 16 */
                char buf[3];
                buf[0] = static_cast<char>(0xcd); _msgpack_store16(&buf[1], static_cast<uint16_t>(d));
                append_buffer(buf, 3);
            }
        } else {
            if(d < (1LL<<32)) {
                /* unsigned 32 */
                char buf[5];
                buf[0] = static_cast<char>(0xce); _msgpack_store32(&buf[1], static_cast<uint32_t>(d));
                append_buffer(buf, 5);
            } else {
                /* unsigned 64 */
                char buf[9];
                buf[0] = static_cast<char>(0xcf); _msgpack_store64(&buf[1], d);
                append_buffer(buf, 9);
            }
        }
    }
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#endif /* msgpack/pack.hpp */
