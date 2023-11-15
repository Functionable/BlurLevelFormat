#pragma once

#include "stringconversionhelper.hpp"

#include "../enums.hpp"
#include "../types.hpp"
#include "../string.hpp"

#include <string>
#include <cstring>
#include <type_traits>

namespace blf
{
    // This file contains functions/templates/classes which
    // are responsible for (de)serializing data.
    template<typename T, typename Enable = void>
    struct ConversionHelper
    {
        inline static void serialize(const T* data, char* location)
        {
            *(T*)(location) = *data;
        }

        inline static void deserialize(T* instance, const char* data)
        {
            *instance = *(T*)(data);
        }

        inline static int measure(const T* data)
        {
            return typeSize(infer<T>::type);
        }
        
        inline static int measureData(const char* data)
        {
            return typeSize(infer<T>::type);
        }
    };

    template<typename T>
    struct ConversionHelper<T, std::enable_if_t<is_string_v<T>>>
    {
        inline static void serialize(const T* data, char* location)
        {
            *(stringsize_t*)(location) = StringConversionHelper<T>::measure(data);
            memcpy(location+sizeof(stringsize_t), StringConversionHelper<T>::buffer(data), StringConversionHelper<T>::measure(data));
        }

        inline static void deserialize(T* instance, const char* data)
        {
            const stringsize_t length = *(stringsize_t*)(data);
            *instance = StringConversionHelper<T>::create(data + sizeof(stringsize_t), length);
        }

        inline static int measure(const T* data)
        {
            return sizeof(stringsize_t) + StringConversionHelper<T>::measure(data);
        }

        inline static int measureData(const char* data)
        {
            return sizeof(stringsize_t) + *(stringsize_t*)(data);
        }
    };

    template<typename T>
    struct ConversionHelper<T, std::enable_if_t<is_class_v<T>>>
    {

    };

    template<typename T>
    inline constexpr void serialize(const T* data, char* location)
    {
        ConversionHelper<T>::serialize(data, location);
    }

    template<typename T>
    inline constexpr void deserialize(T* instance, const char* data)
    {
        ConversionHelper<T>::deserialize(instance, data);
    }

    template<typename T>
    inline constexpr int measure(const T* data)
    {
        return ConversionHelper<T>::measure(data);
    }

    template<typename T>
    inline constexpr int measureData(const char* data)
    {
        return ConversionHelper<T>::measureData(data);
    }

    template<typename T>
    T copyFrom(const char* data);
}