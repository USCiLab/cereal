#ifndef CEREAL_EXTERNAL_TO_STRING_HPP_
#define CEREAL_EXTERNAL_TO_STRING_HPP_

#include <cstdlib>
#include <string>
#include <sstream>

namespace std
{
    inline long double strtold(const char * str, char ** str_end)
    {
        return strtod(str, str_end);
    }

    template<typename T>
    std::string to_string(T const& data)
    {
        std::ostringstream ss;
        ss << data;
        return ss.str();
    }

    inline int stoi(const std::string& str, std::size_t* pos = 0, int base = 10)
    {
        return strtol(str.c_str(), nullptr, base);
    }

    inline long stol(const std::string& str, std::size_t* pos = 0, int base = 10)
    {
        return strtol(str.c_str(), nullptr, base);
    }

    inline long long stoll(const std::string& str, std::size_t* pos = 0, int base = 10)
    {
        return strtoll(str.c_str(), nullptr, base);
    }

    inline unsigned long stoul(const std::string& str, std::size_t* pos = 0, int base = 10)
    {
        return strtoul(str.c_str(), nullptr, base);
    }

    inline unsigned long long stoull(const std::string& str, std::size_t* pos = 0, int base = 10)
    {
        return strtoull(str.c_str(), nullptr, base);
    }

    inline float stof(const std::string& str, std::size_t* pos = 0)
    {
        return strtof(str.c_str(), nullptr);
    }

    inline double stod(const std::string& str, std::size_t* pos = 0)
    {
        return strtod(str.c_str(), nullptr);
    }

    inline long double stold(const std::string& str, std::size_t* pos = 0)
    {
        return strtold(str.c_str(), nullptr);
    }
}

#endif // CEREAL_EXTERNAL_TO_STRING_HPP_
