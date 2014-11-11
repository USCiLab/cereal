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
#ifndef MSGPACK_OBJECT_HPP
#define MSGPACK_OBJECT_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"
#include "msgpack/pack.hpp"
#include "msgpack/zone.hpp"
#include "msgpack/adaptor/int_fwd.hpp"
#include "msgpack/adaptor/bool_fwd.hpp"
#include "msgpack/adaptor/char_ptr_fwd.hpp"
#include "msgpack/adaptor/deque_fwd.hpp"
#include "msgpack/adaptor/fixint_fwd.hpp"
#include "msgpack/adaptor/float_fwd.hpp"
#include "msgpack/adaptor/int_fwd.hpp"
#include "msgpack/adaptor/list_fwd.hpp"
#include "msgpack/adaptor/map_fwd.hpp"
#include "msgpack/adaptor/msgpack_tuple_fwd.hpp"
#include "msgpack/adaptor/nil_fwd.hpp"
#include "msgpack/adaptor/pair_fwd.hpp"
#include "msgpack/adaptor/raw_fwd.hpp"
#include "msgpack/adaptor/set_fwd.hpp"
#include "msgpack/adaptor/string_fwd.hpp"
#include "msgpack/adaptor/vector_fwd.hpp"
#include "msgpack/adaptor/vector_char_fwd.hpp"
#include "msgpack/adaptor/tr1/unordered_map_fwd.hpp"
#include "msgpack/adaptor/tr1/unordered_set_fwd.hpp"

#if !defined(MSGPACK_USE_CPP03)
#include "adaptor/cpp11/array_fwd.hpp"
#include "adaptor/cpp11/array_char_fwd.hpp"
#include "adaptor/cpp11/forward_list_fwd.hpp"
#include "adaptor/cpp11/tuple_fwd.hpp"
#endif // !defined(MSGPACK_USE_CPP03)

#include <string.h>
#include <stdexcept>
#include <typeinfo>
#include <limits>
#include <ostream>

namespace msgpack {

MSGPACK_API_VERSION_NAMESPACE(v1) {

struct object::with_zone : object {
    with_zone(msgpack::zone& zone) : zone(zone) { }
    msgpack::zone& zone;
private:
    with_zone();
};

struct object::implicit_type {
    implicit_type(object const& o) : obj(o) { }
    ~implicit_type() { }

    template <typename T>
    operator T() { return obj.as<T>(); }

private:
    object const& obj;
};

inline object const& operator>> (object const& o, object& v)
{
    v = o;
    return o;
}

template <typename T>
inline object const& operator>> (object const& o, T& v)
{
    // If you get a error 'class your_class has no member named 'msgpack_unpack',
    // check the following:
    // 1. The class your_class should have MSGPACK_DEFINE macro or
    //
    // 2. The class your_class should have the following operator declaration and
    //    definition:
    //    inline object const& operator>> (object const& o, std::string& v)
    //
    //    See 3.
    //
    // 3. #include "msgpack.hpp" too early.
    //    Replace msgpack.hpp with msgpack_fwd.hpp, then,
    //    place operator declarations as follows:
    //
    //    namespace msgpack {
    //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    //    object const& operator>> (object const& o, std::string& v);
    //    }
    //    }
    //
    //    then, #include "msgpack.hpp", finally place the operator definitions as follows:
    //
    //    namespace msgpack {
    //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    //    object const& operator>> (object const& o, std::string& v) {
    //        // converting operations here
    //    }
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    v.msgpack_unpack(o.convert());
    return o;
}

// obsolete
template <typename Type>
class define : public Type {
public:
    typedef Type msgpack_type;
    typedef define<Type> define_type;

    define() {}
    define(const msgpack_type& v) : msgpack_type(v) {}

    template <typename Packer>
    void msgpack_pack(Packer& o) const
    {
        msgpack::operator<<(o, static_cast<const msgpack_type&>(*this));
    }

    void msgpack_unpack(object const& o)
    {
        msgpack::operator>>(o, static_cast<msgpack_type&>(*this));
    }
};

namespace detail {
template <typename Stream, typename T>
struct packer_serializer {
    static packer<Stream>& pack(packer<Stream>& o, const T& v) {
        // If you get a error 'const class your_class has no member named 'msgpack_pack',
        // check the following:
        // 1. The class your_class should have MSGPACK_DEFINE macro or
        //
        // 2. The class your_class should have the following operator declaration and
        //    definition:
        //
        //    namespace msgpack {
        //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
        //    template <typename Stream>
        //    inline packer<Stream>& operator<< (packer<Stream>& o, const your_class& v)
        //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
        //    } // namespace msgpack
        //
        //    See 3.
        //
        // 3. #include "msgpack.hpp" too early.
        //    Replace msgpack.hpp with msgpack_fwd.hpp, then,
        //    place operator declarations as follows:
        //
        //    namespace msgpack {
        //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
        //    template <typename Stream>
        //    packer<Stream>& operator<< (packer<Stream>& o, const your_class& v);
        //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
        //    } // namespace msgpack
        //
        //    then, #include "msgpack.hpp", finally place the operator definitions as follows:
        //
        //    template <typename Stream>
        //    inline packer<Stream>& operator<< (packer<Stream>& o, const your_class& v) {
        //         // packing operation
        //    }
        //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
        //    } // namespace msgpack
        //
        v.msgpack_pack(o);
        return o;
    }
};
}

// serialize operator
template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const T& v)
{
    return detail::packer_serializer<Stream, T>::pack(o, v);
}

// deconvert operator
template <typename T>
inline void operator<< (object::with_zone& o, const T& v)
{
    // If you get a error 'const class your_class has no member named 'msgpack_object',
    // check the following:
    // 1. The class your_class should have MSGPACK_DEFINE macro or
    //
    // 2. The class your_class should have the following operator declaration and
    //    definition:
    //
    //    namespace msgpack {
    //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    //    void operator<< (object::with_zone& o, const your_class& v);
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    //    See 3.
    //
    // 3. #include "msgpack.hpp" too early.
    //    Replace msgpack.hpp with msgpack_fwd.hpp, then,
    //    place operator declarations as follows:
    //
    //    namespace msgpack {
    //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    //    void operator<< (object::with_zone& o, const your_class& v);
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    //    then, #include "msgpack.hpp", finally place the operator definitions as follows:
    //
    //    void operator<< (object::with_zone& o, const your_class& v) {
    //         // set object attributes using v.
    //    }
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    v.msgpack_object(static_cast<object*>(&o), o.zone);
}

template <typename Stream>
template <typename T>
inline packer<Stream>& packer<Stream>::pack(const T& v)
{
    msgpack::operator<<(*this, v);
    return *this;
}

inline bool operator==(const object& x, const object& y)
{
    if(x.type != y.type) { return false; }

    switch(x.type) {
    case type::NIL:
        return true;

    case type::BOOLEAN:
        return x.via.boolean == y.via.boolean;

    case type::POSITIVE_INTEGER:
        return x.via.u64 == y.via.u64;

    case type::NEGATIVE_INTEGER:
        return x.via.i64 == y.via.i64;

    case type::DOUBLE:
        return x.via.dec == y.via.dec;

    case type::STR:
        return x.via.str.size == y.via.str.size &&
            memcmp(x.via.str.ptr, y.via.str.ptr, x.via.str.size) == 0;

    case type::BIN:
        return x.via.bin.size == y.via.bin.size &&
            memcmp(x.via.bin.ptr, y.via.bin.ptr, x.via.bin.size) == 0;

    case type::EXT:
        return x.via.ext.size == y.via.ext.size &&
            memcmp(x.via.ext.ptr, y.via.ext.ptr, x.via.ext.size) == 0;

    case type::ARRAY:
        if(x.via.array.size != y.via.array.size) {
            return false;
        } else if(x.via.array.size == 0) {
            return true;
        } else {
            object* px = x.via.array.ptr;
            object* const pxend = x.via.array.ptr + x.via.array.size;
            object* py = y.via.array.ptr;
            do {
                if(!(*px == *py)) {
                    return false;
                }
                ++px;
                ++py;
            } while(px < pxend);
            return true;
        }

    case type::MAP:
        if(x.via.map.size != y.via.map.size) {
            return false;
        } else if(x.via.map.size == 0) {
            return true;
        } else {
            object_kv* px = x.via.map.ptr;
            object_kv* const pxend = x.via.map.ptr + x.via.map.size;
            object_kv* py = y.via.map.ptr;
            do {
                if(!(px->key == py->key) || !(px->val == py->val)) {
                    return false;
                }
                ++px;
                ++py;
            } while(px < pxend);
            return true;
        }

    default:
        return false;
    }
}

template <typename T>
inline bool operator==(const object& x, const T& y)
try {
    return x == object(y);
} catch (msgpack::type_error&) {
    return false;
}

inline bool operator!=(const object& x, const object& y)
{ return !(x == y); }

template <typename T>
inline bool operator==(const T& y, const object x)
{ return x == y; }

template <typename T>
inline bool operator!=(const object& x, const T& y)
{ return !(x == y); }

template <typename T>
inline bool operator!=(const T& y, const object& x)
{ return x != y; }


inline object::implicit_type object::convert() const
{
    return implicit_type(*this);
}

template <typename T>
inline void object::convert(T& v) const
{
    msgpack::operator>>(*this, v);
}

template <typename T>
inline void object::convert(T* v) const
{
    convert(*v);
}

template <typename T>
inline T object::as() const
{
    T v;
    convert(v);
    return v;
}


inline object::object()
{
    type = type::NIL;
}

template <typename T>
inline object::object(const T& v)
{
    // If you get a error 'no matching function call to
    // operator<<(msgpack::v?::object::object(const T&)
    // [with T = your_class]'
    //
    // check the following:
    // 1. The class your_class should have MSGPACK_DEFINE macro or
    //
    // 2. The class your_class should have the following operator declaration and
    //    definition:
    //
    //    namespace msgpack {
    //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    //    void operator<< (object& o, const your_class& v);
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    //    See 3.
    //
    // 3. #include "msgpack.hpp" too early.
    //    Replace msgpack.hpp with msgpack_fwd.hpp, then,
    //    place operator declarations as follows:
    //
    //    namespace msgpack {
    //    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    //    void operator<< (object& o, const your_class& v);
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    //    then, #include "msgpack.hpp", finally place the operator definitions as follows:
    //
    //    void operator<< (object& o, const your_class& v) {
    //         // set object attributes using v.
    //    }
    //    } // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
    //    } // namespace msgpack
    //
    msgpack::operator<<(*this, v);
}

template <typename T>
inline object& object::operator=(const T& v)
{
    *this = object(v);
    return *this;
}

template <typename T>
object::object(const T& v, zone& z)
{
    with_zone oz(z);
    msgpack::operator<<(oz, v);
    type = oz.type;
    via = oz.via;
}

template <typename T>
object::object(const T& v, zone* z)
{
    with_zone oz(*z);
    msgpack::operator<<(oz, v);
    type = oz.type;
    via = oz.via;
}


inline object::object(msgpack_object o)
{
    // FIXME beter way?
    ::memcpy(this, &o, sizeof(o));
}

inline void operator<< (object& o, msgpack_object v)
{
    // FIXME beter way?
    ::memcpy(&o, &v, sizeof(v));
}

inline object::operator msgpack_object() const
{
    // FIXME beter way?
    msgpack_object obj;
    ::memcpy(&obj, this, sizeof(obj));
    return obj;
}


// obsolete
template <typename T>
inline void convert(T& v, object const& o)
{
    o.convert(v);
}

// obsolete
template <typename Stream, typename T>
inline void pack(packer<Stream>& o, const T& v)
{
    o.pack(v);
}

// obsolete
template <typename Stream, typename T>
inline void pack_copy(packer<Stream>& o, T v)
{
    pack(o, v);
}


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const object& v)
{
    switch(v.type) {
    case type::NIL:
        o.pack_nil();
        return o;

    case type::BOOLEAN:
        if(v.via.boolean) {
            o.pack_true();
        } else {
            o.pack_false();
        }
        return o;

    case type::POSITIVE_INTEGER:
        o.pack_uint64(v.via.u64);
        return o;

    case type::NEGATIVE_INTEGER:
        o.pack_int64(v.via.i64);
        return o;

    case type::DOUBLE:
        o.pack_double(v.via.dec);
        return o;

    case type::STR:
        o.pack_str(v.via.str.size);
        o.pack_str_body(v.via.str.ptr, v.via.str.size);
        return o;

    case type::BIN:
        o.pack_bin(v.via.bin.size);
        o.pack_bin_body(v.via.bin.ptr, v.via.bin.size);
        return o;

    case type::EXT:
        o.pack_ext(v.via.ext.size, v.via.ext.type());
        o.pack_ext_body(v.via.ext.data(), v.via.ext.size);
        return o;

    case type::ARRAY:
        o.pack_array(v.via.array.size);
        for(object* p(v.via.array.ptr),
                * const pend(v.via.array.ptr + v.via.array.size);
                p < pend; ++p) {
            msgpack::operator<<(o, *p);
        }
        return o;

    case type::MAP:
        o.pack_map(v.via.map.size);
        for(object_kv* p(v.via.map.ptr),
                * const pend(v.via.map.ptr + v.via.map.size);
                p < pend; ++p) {
            msgpack::operator<<(o, p->key);
            msgpack::operator<<(o, p->val);
        }
        return o;

    default:
        throw type_error();
    }
}

inline std::ostream& operator<< (std::ostream& s, const object& o)
{
    switch(o.type) {
    case type::NIL:
        s << "nil";
        break;

    case type::BOOLEAN:
        s << (o.via.boolean ? "true" : "false");
        break;

    case type::POSITIVE_INTEGER:
        s << o.via.u64;
        break;

    case type::NEGATIVE_INTEGER:
        s << o.via.i64;
        break;

    case type::DOUBLE:
        s << o.via.dec;
        break;

    case type::STR:
        (s << '"').write(o.via.str.ptr, o.via.str.size) << '"';
        break;

    case type::BIN:
        (s << '"').write(o.via.bin.ptr, o.via.bin.size) << '"';
        break;

    case type::EXT:
        s << "EXT";

    case type::ARRAY:
        s << "[";
        if(o.via.array.size != 0) {
            object* p(o.via.array.ptr);
            s << *p;
            ++p;
            for(object* const pend(o.via.array.ptr + o.via.array.size);
                    p < pend; ++p) {
                s << ", " << *p;
            }
        }
        s << "]";
        break;

    case type::MAP:
        s << "{";
        if(o.via.map.size != 0) {
            object_kv* p(o.via.map.ptr);
            s << p->key << "=>" << p->val;
            ++p;
            for(object_kv* const pend(o.via.map.ptr + o.via.map.size);
                    p < pend; ++p) {
                s << ", " << p->key << "=>" << p->val;
            }
        }
        s << "}";
        break;

    default:
        // FIXME
        s << "#<UNKNOWN " << static_cast<uint16_t>(o.type) << ">";
    }
    return s;
}

}  // MSGPACK_API_VERSION_NAMESPACE(v1)

}  // namespace msgpack

#include "msgpack/type.hpp"

#endif /* msgpack/object.hpp */
