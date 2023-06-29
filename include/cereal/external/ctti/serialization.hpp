#ifndef CTTI_SERIALIZATION_HPP
#define CTTI_SERIALIZATION_HPP

#include <ctti/symbol.hpp>
#include <ctti/model.hpp>
#include <ctti/detail/nlohmann_json.hpp>
#include <vector>
#include <unordered_map>
#include <array>
#include <tuple>

namespace ctti
{

namespace serialization
{

template<typename Formatter, typename Output, typename T>
void serialize(Formatter formatter, Output& output, const T& value);

template<typename Writer, typename T>
void serialize(Writer writer, const T& value);

template<typename Reader, typename T>
void deserialize(Reader reader, T& value);

namespace detail
{
    template<typename T, bool IsEnum = std::is_enum<T>(), bool EmptyModel = ctti::meta::list_size<ctti::get_model<T>>() == 0>
    struct serialize
    {
        using Model = ctti::get_model<T>;

        template<typename Writer>
        struct loop_body
        {
            Writer* writer;
            const T* value;

            template<typename SymbolIdentity, typename Index>
            void operator()(SymbolIdentity, Index)
            {
                using Symbol = ctti::meta::type_t<SymbolIdentity>;
                constexpr std::size_t index = Index();

                writer->value(Symbol::symbol().str(), ctti::get_member_value<Symbol>(*value));

                if(index + 1 < ctti::meta::list_size<Model>())
                {
                    writer->value_separator();
                }
            }
        };

        template<typename Writer>
        static void apply(Writer& writer, const T& value)
        {
            loop_body<Writer> loop_body{&writer, &value};

            writer.begin_object(value);
                ctti::meta::foreach<Model>(loop_body);
            writer.end_object(value);
        }
    };

    template<typename T>
    struct serialize<T, true, false>
    {
        using Model = ctti::get_model<T>;

        template<typename Writer>
        struct loop_body
        {
            Writer* writer;
            const T* value;

            template<typename SymbolIdentity, typename Index>
            void operator()(SymbolIdentity, Index)
            {
                using Symbol = ctti::meta::type_t<SymbolIdentity>;

                if(Symbol::template get_member<T>() == *value)
                {
                    // Found the enumeration value with this value
                    writer->raw_value(Symbol::symbol().str());
                }
            }
        };

        template<typename Writer>
        static void apply(Writer& writer, const T& value)
        {
            loop_body<Writer> loop_body{&writer, &value};
            ctti::meta::foreach<Model>(loop_body);
        }
    };

    template<typename T, bool IsEnum>
    struct serialize<T, IsEnum, true>
    {
        template<typename Writer>
        static void apply(Writer& writer, const T& value)
        {
            writer.value(value);
        }
    };

    template<typename T, bool IsEnum = std::is_enum<T>::value, bool HasModel = ctti::has_model<T>::value>
    struct deserialize
    {
        using Model = ctti::get_model<T>;

        template<typename Reader>
        struct loop_body
        {
            Reader* reader;
            T* value;

            template<typename SymbolIdentity, typename Index>
            void operator()(SymbolIdentity, Index)
            {
                using Symbol = ctti::meta::type_t<SymbolIdentity>;
                reader->value(Symbol::symbol().str(), ctti::get_member_value<Symbol>(*value));
            }
        };

        template<typename Reader>
        static void apply(Reader& reader, T& value)
        {
            loop_body<Reader> loop_body{&reader, &value};
            ctti::meta::foreach<Model>(loop_body);
        }
    };

    template<typename T>
    struct deserialize<T, true, true>
    {
        using Model = ctti::get_model<T>;

        template<typename InputValue, typename ValueType = T>
        struct loop_body
        {
            T* value;
            InputValue input_value;

            template<typename SymbolIdentity, typename Index>
            void operator()(SymbolIdentity, Index)
            {
                using Symbol = ctti::meta::type_t<SymbolIdentity>;

                if(static_cast<typename std::underlying_type<T>::type>(Symbol::template get_member<T>()) ==
                   static_cast<typename std::underlying_type<T>::type>(input_value))
                {
                    *value = Symbol::template get_member<T>();
                }
            }
        };

        template<typename ValueType>
        struct loop_body<std::string, ValueType>
        {
            T* value;
            std::string input_value;

            template<typename SymbolIdentity, typename Index>
            void operator()(SymbolIdentity, Index)
            {
                using Symbol = ctti::meta::type_t<SymbolIdentity>;

                if(Symbol::symbol().str() == input_value)
                {
                    *value = Symbol::template get_member<T>();
                }
            }
        };

        struct callback
        {
            template<typename InputValue>
            T operator()(const InputValue& input_value)
            {
                T output_value{};
                ctti::meta::foreach<Model>(loop_body<InputValue>{&output_value, input_value});
                return output_value;
            }
        };

        template<typename Reader>
        static void apply(Reader& reader, T& value)
        {
            reader.enum_value(value, callback());
        }
    };

    template<typename T, bool IsEnum>
    struct deserialize<T, IsEnum, false>
    {
        template<typename Reader>
        static void apply(Reader& reader, T& value)
        {
            reader.value(value);
        }
    };
}

template<typename Formatter, typename Output>
struct Writer
{
    Writer(Formatter formatter, Output& output) :
        _formatter{formatter},
        _output{&output}
    {}

    void value_separator()
    {
        _formatter.value_separator(*_output);
    }

    template<typename T>
    void raw_value(const T& value)
    {
        _formatter.raw_value(*_output, value);
    }

    template<typename T>
    void value(const T& value)
    {
        _formatter.value(*_output, value);
    }

    template<typename T>
    void value(const std::string& name, const T& value)
    {
        _formatter.value(*_output, name, value);
    }

    template<typename T>
    void begin_object(const T& object)
    {
        _formatter.begin_object(*_output, object);
    }

    template<typename T>
    void end_object(const T& object)
    {
        _formatter.end_object(*_output, object);
    }

private:
    Formatter _formatter;
    Output* _output;
};

template<typename Formatter, typename Output>
Writer<Formatter, Output> make_writer(Formatter formatter, Output& output)
{
    return {formatter, output};
}

struct ostream_otuput
{
    ostream_otuput(std::ostream& os) :
        os(&os)
    {}

    template<typename T>
    void write(const T& value)
    {
        (*os) << value;
    }

private:
    std::ostream* os;
};

struct json_formatter
{
    template<typename Output, typename T>
    void raw_value(Output& output, const T& value)
    {
        output.write(value);
    }

    template<typename Output>
    void value_separator(Output& out)
    {
        out.write(", ");
    }

    template<typename Output, typename T>
    void begin_object(Output& out, const T&)
    {
        out.write("{");
    }

    template<typename Output, typename T>
    void end_object(Output& out, const T&)
    {
        out.write("}");
    }

    template<typename Output, typename T>
    void value(Output& out, const std::string& name, const T& value)
    {
        out.write("\""); out.write(name); out.write("\": "); this->value(out, value);
    }

    template<typename Output, typename T>
    void value(Output& out, const T& value)
    {
        write_value(out, value, ctti::meta::bool_<ctti::meta::list_size<ctti::get_model<T>>() == 0>());
    }

    template<typename Output>
    void value(Output& out, const std::string& value)
    {
        out.write("\""); out.write(value); out.write("\"");
    }

    template<typename Output, typename T>
    void value(Output& out, const std::vector<T>& vector)
    {
        out.write("[");

        for(std::size_t i = 0; i < vector.size(); ++i)
        {
            ctti::serialization::serialize(*this, out, vector[i]);

            if(i + 1 < vector.size())
            {
                out.write(", ");
            }
        }

        out.write("]");
    }

    template<typename Output, typename T, std::size_t Size>
    void value(Output& out, const std::array<T, Size>& array)
    {
        out.write("[");

        for(std::size_t i = 0; i < array.size(); ++i)
        {
            ctti::serialization::serialize(*this, out, array[i]);

            if(i + 1 < array.size())
            {
                out.write(", ");
            }
        }

        out.write("]");
    }

    template<typename Output, typename Key, typename Value, typename Hash>
    void value(Output& out, const std::unordered_map<Key, Value, Hash>& map)
    {
        out.write("[");
        std::size_t i = 0;

        for(const auto& keyValue : map)
        {
            out.write("{");
            ctti::serialization::serialize(*this, out, keyValue.first);
            out.write(": ");
            ctti::serialization::serialize(*this, out, keyValue.second);
            out.write("}");

            if(i + 1 < map.size())
            {
                out.write(", ");
            }

            ++i;
        }

        out.write("]");
    }

    template<typename Output, typename... Ts>
    void value(Output& out, const std::tuple<Ts...>& tuple)
    {
        out.write("(");
        ctti::meta::foreach<std::tuple<Ts...>>(tuple_loop_body_t<Output, Ts...>{&tuple, this, &out});
        out.write(")");
    }

private:
    template<typename Output, typename... Ts>
    struct tuple_loop_body_t
    {
        const std::tuple<Ts...>* tuple;
        json_formatter* self;
        Output* out;

        template<typename T, std::size_t Index>
        void operator()(ctti::meta::identity<T>, ctti::meta::size_t<Index>) const
        {
            ctti::serialization::serialize(*self, *out, std::get<Index>(*tuple));

            if(Index + 1 < sizeof...(Ts))
            {
                out->write(", ");
            }
        }
    };

    template<typename Output, typename T>
    void write_value(Output& out, const T& value, ctti::meta::true_)
    {
        out.write(value);
    }

    template<typename Output, typename T>
    void write_value(Output& out, const T& value, ctti::meta::false_)
    {
        ctti::serialization::serialize(*this, out, value);
    }
};

struct json_writer
{
    json_writer(nlohmann::json& current_object) :
        _current_object{&current_object}
    {}

    template<typename T>
    ctti::meta::enable_if_t<ctti::has_model<T>::value> value(const std::string& name, const T& value)
    {
        ctti::serialization::serialize(json_writer((*_current_object)[name]), value);
    }

    template<typename T>
    ctti::meta::enable_if_t<!ctti::has_model<T>::value> value(const std::string& name, const T& value)
    {
        (*_current_object)[name] = nlohmann::json(value);
    }

    template<typename T>
    void raw_value(const T& value)
    {
        *_current_object = nlohmann::json(value);
    }

    template<typename T>
    ctti::meta::enable_if_t<ctti::has_model<T>::value> value(const T& value)
    {
        ctti::serialization::serialize(json_writer(*_current_object), value);
    }

    template<typename T>
    ctti::meta::enable_if_t<!ctti::has_model<T>::value> value(const T& value)
    {
        raw_value(value);
    }

    template<typename T>
    void begin_object(const T&)
    {
    }

    template<typename T>
    void end_object(const T&)
    {
    }

    void value_separator() {}

private:
    nlohmann::json* _current_object;
};

struct json_reader
{
    json_reader(const nlohmann::json& json) :
        _current_object{&json}
    {}

    template<typename T>
    ctti::meta::enable_if_t<ctti::has_model<T>::value> value(T& value)
    {
        ctti::serialization::deserialize(json_reader(*_current_object), value);
    }

    template<typename T>
    ctti::meta::enable_if_t<!ctti::has_model<T>::value> value(T& value)
    {
        value = _current_object->get<T>();
    }

    template<typename T, typename Function>
    void enum_value(T& value, Function callback)
    {
        if(_current_object->is_number())
        {
            value = callback(_current_object->get<typename std::underlying_type<T>::type>());
        }
        else if(_current_object->is_string())
        {
            value = callback(_current_object->get<std::string>());
        }
    }

    template<typename T>
    ctti::meta::enable_if_t<ctti::has_model<T>::value> value(const std::string& name, T& value)
    {
        ctti::serialization::deserialize(json_reader((*_current_object)[name]), value);
    }

    template<typename T>
    ctti::meta::enable_if_t<!ctti::has_model<T>::value> value(const std::string& name, T& value)
    {
        auto subobject = (*_current_object)[name];
        value = subobject.get<T>();
    }

    template<typename Key, typename Value, typename Hash>
    void value(const std::string& name, std::unordered_map<Key, Value, Hash>& map)
    {
        value((*_current_object)[name], map);
    }

    template<typename Key, typename Value, typename Hash>
    void value(std::unordered_map<Key, Value, Hash>& map)
    {
        value(*_current_object, map);
    }

private:
    const nlohmann::json* _current_object;

    template<typename Key, typename Value, typename Hash>
    void value(const nlohmann::json& json, std::unordered_map<Key, Value, Hash>& map)
    {
        assert(json.is_array());

        for(const auto& key_value_object : json)
        {
            // maps are serialized as key, value arrays, where key value pairs are arrays too
            assert(key_value_object.is_array());
            assert(key_value_object.size() == 2);

            const auto& key = key_value_object[0];
            const auto& value = key_value_object[1];

            Key deserialized_key;
            Value deserialized_value;
            ctti::serialization::deserialize(json_reader(key), deserialized_key);
            ctti::serialization::deserialize(json_reader(value), deserialized_value);

            map[std::move(deserialized_key)] = std::move(deserialized_value);
        }
    }
};

struct enum_from_string_reader
{
    enum_from_string_reader(const std::string& input) :
        _input{input}
    {}

    template<typename T, typename Callback>
    void enum_value(T& value, Callback callback)
    {
        value = callback(_input);
    }

private:
    std::string _input;
};

struct enum_to_string_writer
{
    enum_to_string_writer(std::string& output) :
        _output{&output}
    {}

    void raw_value(const std::string& enum_value)
    {
        *_output = enum_value;
    }

private:
    std::string* _output;
};

template<typename Enum>
ctti::meta::enable_if_t<ctti::has_model<Enum>::value && std::is_enum<Enum>::value, std::string> enum_to_string(const Enum value)
{
    std::string output;
    ctti::serialization::serialize(ctti::serialization::enum_to_string_writer(output), value);
    return output;
}

template<typename Enum>
ctti::meta::enable_if_t<ctti::has_model<Enum>::value && std::is_enum<Enum>::value, Enum> enum_from_string(const std::string& enum_string)
{
    Enum enum_value{};
    ctti::serialization::deserialize(ctti::serialization::enum_from_string_reader(enum_string), enum_value);
    return enum_value;
}

template<typename Writer, typename T>
void serialize(Writer writer, const T& value)
{
    ctti::serialization::detail::serialize<T>::apply(writer, value);
}

template<typename Formatter, typename Output, typename T>
void serialize(Formatter formatter, Output& output, const T& value)
{
    auto writer = ctti::serialization::make_writer(formatter, output);
    ctti::serialization::serialize(writer, value);
}

template<typename Formatter, typename Output, typename T>
void serialize(Formatter formatter, Output output, const T& value)
{
    auto writer = ctti::serialization::make_writer(formatter, output);
    ctti::serialization::serialize(writer, value);
}

template<typename Reader, typename T>
void deserialize(Reader reader, T& value)
{
    ctti::serialization::detail::deserialize<T>::apply(reader, value);
}

namespace detail
{
    template<typename T, bool HasModel = ctti::meta::list_size<ctti::get_model<T>>() == 0, typename = void>
    struct is_default_ostream_serializable : public ctti::meta::false_ {};

    template<typename T>
    struct is_default_ostream_serializable<T, false, ctti::meta::void_t<decltype(T::ctti_ostream_print())>> : public ctti::meta::true_ {};

    template<typename T>
    struct is_default_ostream_serializable<T, false, ctti::meta::void_t<decltype(std::declval<T>().ctti_ostream_print())>> : public ctti::meta::true_ {};

    template<typename T>
    struct is_default_ostream_serializable<T, false, ctti::meta::void_t<decltype(ctti_ostream_print(std::declval<ctti::type_tag<T>>()))>> : public ctti::meta::true_ {};
}

}

}

template<typename T>
ctti::meta::enable_if_t<ctti::serialization::detail::is_default_ostream_serializable<T>::value, std::ostream&> operator<<(std::ostream& os, const T& value)
{
    ctti::serialization::serialize(ctti::serialization::json_formatter(), ctti::serialization::ostream_otuput(os), value);
    return os;
}

#endif // CTTI_SERIALIZATION_HPP
