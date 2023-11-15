#pragma once

#include "../enums.hpp"
#include "../string.hpp"

#include <string>
#include <type_traits>

namespace blf
{
    template<typename T, typename Enable = void, typename = std::enable_if_t<is_string_v<T>>>
    struct StringConversionHelper
    {
        /**
         * Measures the length of the string buffer - NULL byte.
         */
        inline static int measure(const T* data);

        /**
         * Retrieves the internal char buffer of the string.
         */
        inline static const char* buffer(const T* data);

        /**
         * Creates a new instance of std::string from a given char* buffer and length.
         */
        inline static T create(const char* buffer, size_t length);
    };

    template<>
    struct StringConversionHelper<std::string>
    {
        inline static int measure(const std::string* data)
        {
            return data->length();
        }

        inline static const char* buffer(const std::string* data)
        {
            return data->c_str();
        }

        inline static std::string create(const char* buffer, size_t length)
        {
            return std::string(buffer, 0, length);
        }
    };

    template<>
    struct StringConversionHelper<String>
    {
        inline static int measure(const String* data)
        {
            return data->getLength();
        }

        inline static const char* buffer(const String* data)
        {
            return data->getBuffer();
        }

        inline static String create(const char* buffer, size_t length)
        {
            // TODO: make better constructor for String?
            return String(buffer);
        }
    };
}