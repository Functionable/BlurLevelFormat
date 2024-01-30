#pragma once

#include "serializationhelper.hpp"
#include "serializationcontext.hpp"

#include "../enums.hpp"
#include "../types.hpp"
#include "../string.hpp"
#include "../objecttable.hpp"

#include "../object/objectdefinition.hpp"

#include <string>
#include <cstring>
#include <type_traits>

namespace blf
{
    template<typename T>
    inline constexpr void serialize(SerializationContext& ctx, const T* data, char* location)
    {
        SerializationHelper<T>::serialize(ctx, data, location);
    }

    template<typename T>
    inline constexpr void deserialize(SerializationContext& ctx, T* instance, const char* data)
    {
        SerializationHelper<T>::deserialize(ctx, instance, data);
    }

    template<typename T>
    inline constexpr int serializedLength(SerializationContext& ctx, const T* data)
    {
        return SerializationHelper<T>::serializedLength(ctx, data);
    }

    template<typename T>
    inline constexpr int dataLength(SerializationContext& ctx, const char* data)
    {
        return SerializationHelper<T>::dataLength(ctx, data);
    }

    void serialize(SerializationContext& ctx, 
            const BLF_TYPE type, const char* data, char* location);

    int dataLength(SerializationContext& ctx, 
            const ObjectDefinition* referenced, const BLF_TYPE type, 
            const char* data);

    void deserialize(SerializationContext& ctx, 
            const ObjectDefinition* referenced, const BLF_TYPE type, 
            char* instance, const char* data);
}