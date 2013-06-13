#pragma once

#include "cereal.hpp"
#include <cassert>
#include <jsoncpp/json/json.h>

namespace cereal
{
  // ######################################################################
  class JSONOutputArchive : public OutputArchive<JSONOutputArchive>
  {
    public:
      JSONOutputArchive(std::ostream & stream) :
        OutputArchive<JSONOutputArchive>(this),
        itsStream(stream)
    { }

      Json::StyledStreamWriter & writer() { return itsWriter; }
      std::ostream & stream() {return itsStream; }

    private:
      std::ostream & itsStream;
      Json::StyledStreamWriter itsWriter;
  };

  // ######################################################################
  class JSONInputArchive : public InputArchive<JSONInputArchive>
  {
    public:
      JSONInputArchive(std::istream & stream) :
        InputArchive<JSONInputArchive>(this),
        itsStream(stream)
    { }

      std::istream & stream() {return itsStream; }

    private:
      std::istream & itsStream;
  };

  //! Saving for POD types to json
  template<class T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type
    save(JSONOutputArchive & ar, T const & t)
    {
      //ar.stream() << t;
    }

  //! Loading for POD types from json
  template<class T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type
    load(JSONInputArchive & ar, T & t)
    {
      //ar.stream() >> t;
    }

  //! Saving for NVP types to json
  template<class T>
    void save(JSONOutputArchive & ar, NameValuePair<T> const & t)
    {
      //ar.stream() << t.name << " : ";
      //
      //ar & t.value;

      //ar.stream() << "\n";
      Json::Value v;
      v[t.name] = t.value;
      ar.writer().write(ar.stream(), v);

      //ar.writer().write(ar.stream(), Json::Value(t.name));
      //ar.writer().write(ar.stream(), Json::Value(t.value));
    }

  //! Loading for NVP types from json
  template<class T>
    void load(JSONInputArchive & ar, NameValuePair<T> t)
    {
      ar & t.value;
    }

  //! Serialization for basic_string types to json
  template<class CharT, class Traits, class Alloc>
    void save(JSONOutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str)
    {
      //ar.stream() << "\"" << str << "\"";
    }

  //! Serialization for basic_string types from json
  template<class CharT, class Traits, class Alloc>
    void load(JSONInputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str)
    {
      //size_t size;
      //ar & size;
      //str.resize(size);
      //ar.load_json(const_cast<CharT*>(str.data()), size * sizeof(CharT));
      //std::cout << "Loading string: " << str << std::endl;
    }
}
