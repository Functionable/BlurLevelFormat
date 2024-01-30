#pragma once

#include "serializationcontext.hpp"
#include "stringserializationhelper.hpp"

#include "../enums.hpp"
#include "../types.hpp"

#include <cstring>

namespace blf
{
    // This file contains functions/templates/classes which
    // are responsible for (de)serializing data.
    template<typename T, typename Enable = void>
    struct SerializationHelper
    {
        inline static void serialize(SerializationContext& ctx, const T* data, char* location)
        {
            *(T*)(location) = *data;
        }

        inline static void deserialize(SerializationContext& ctx, T* instance, const char* data)
        {
            *instance = *(T*)(data);
        }

        inline static int serializedLength(SerializationContext& ctx, const T* data)
        {
            return typeSize(infer<T>::type);
        }
        
        inline static int dataLength(SerializationContext& ctx, const char* data)
        {
            return typeSize(infer<T>::type);
        }
    };

    template<typename T>
    struct SerializationHelper<T, std::enable_if_t<is_string_v<T>>>
    {
        inline static void serialize(SerializationContext& ctx, const T* data, char* location)
        {
            *(stringsize_t*)(location) = StringSerializationHelper<T>::serializedLength(data);
            memcpy(location+sizeof(stringsize_t), StringSerializationHelper<T>::buffer(data), StringSerializationHelper<T>::serializedLength(data));
        }

        inline static void deserialize(SerializationContext& ctx, T* instance, const char* data)
        {
            const stringsize_t length = *(stringsize_t*)(data);
            *instance = StringSerializationHelper<T>::create(data + sizeof(stringsize_t), length);
        }

        inline static int serializedLength(SerializationContext& ctx, const T* data)
        {
            return sizeof(stringsize_t) + StringSerializationHelper<T>::serializedLength(data);
        }

        inline static int dataLength(SerializationContext& ctx, const char* data)
        {
            return sizeof(stringsize_t) + *(stringsize_t*)(data);
        }
    };
}