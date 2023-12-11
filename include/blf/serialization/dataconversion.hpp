#pragma once

#include "serializationcontext.hpp"
#include "stringconversionhelper.hpp"

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
    // This file contains functions/templates/classes which
    // are responsible for (de)serializing data.
    template<typename T, typename Enable = void>
    struct ConversionHelper
    {
        inline static void serialize(SerializationContext& ctx, const T* data, char* location)
        {
            *(T*)(location) = *data;
        }

        inline static void deserialize(SerializationContext& ctx, T* instance, const char* data)
        {
            *instance = *(T*)(data);
        }

        inline static int measure(SerializationContext& ctx, const T* data)
        {
            return typeSize(infer<T>::type);
        }
        
        inline static int measureData(SerializationContext& ctx, const char* data)
        {
            return typeSize(infer<T>::type);
        }
    };

    template<typename T>
    struct ConversionHelper<T, std::enable_if_t<is_string_v<T>>>
    {
        inline static void serialize(SerializationContext& ctx, const T* data, char* location)
        {
            *(stringsize_t*)(location) = StringConversionHelper<T>::measure(data);
            memcpy(location+sizeof(stringsize_t), StringConversionHelper<T>::buffer(data), StringConversionHelper<T>::measure(data));
        }

        inline static void deserialize(SerializationContext& ctx, T* instance, const char* data)
        {
            const stringsize_t length = *(stringsize_t*)(data);
            *instance = StringConversionHelper<T>::create(data + sizeof(stringsize_t), length);
        }

        inline static int measure(SerializationContext& ctx, const T* data)
        {
            return sizeof(stringsize_t) + StringConversionHelper<T>::measure(data);
        }

        inline static int measureData(SerializationContext& ctx, const char* data)
        {
            return sizeof(stringsize_t) + *(stringsize_t*)(data);
        }
    };

    template<typename T>
    struct ConversionHelper<T, std::enable_if_t<is_class_v<T>>>
    {

    };

    template<typename T>
    inline constexpr void serialize(SerializationContext& ctx, const T* data, char* location)
    {
        ConversionHelper<T>::serialize(ctx, data, location);
    }

    template<typename T>
    inline constexpr void deserialize(SerializationContext& ctx, T* instance, const char* data)
    {
        ConversionHelper<T>::deserialize(ctx, instance, data);
    }

    template<typename T>
    inline constexpr int measure(SerializationContext& ctx, const T* data)
    {
        return ConversionHelper<T>::measure(ctx, data);
    }

    template<typename T>
    inline constexpr int measureData(SerializationContext& ctx, const char* data)
    {
        return ConversionHelper<T>::measureData(ctx, data);
    }

    template<typename T>
    inline constexpr int measureRef(SerializationContext& ctx, const T* data)
    {
        //TODO: implement
        return -1;
    }


    /**
     * These functions apply to serializing/deserializing data only based on 
     * a given type enum instead of a given type like in the template-based functions.
     */


    inline constexpr void serialize(SerializationContext& ctx, const BLF_TYPE type, const char* data, char* location)
    {
        ;
    }

    inline constexpr int measureData(SerializationContext& ctx, const ObjectDefinition* referenced, const BLF_TYPE type, const char* data)
    {
        switch(type)
        {
            case TYPE_BYTE: return 1;
            case TYPE_SHORT: return 2;
            case TYPE_FLOAT:
            case TYPE_INT: return 4;
            case TYPE_DOUBLE:
            case TYPE_LONG: return 8;
            case TYPE_STRING:
                return measureData<String>(ctx, data);
            case TYPE_OBJECT:
                return referenced->measureSpan(ctx, data);
            case TYPE_OBJECTREFERENCE:
                throw std::logic_error("Not implemented yet.");
                return -1;

            case TYPE_NULL:
            case TYPE_UNKNOWN:
                return 0;
        }

        return -1;
    }

    inline constexpr void deserialize(SerializationContext& ctx, const ObjectDefinition* referenced, const BLF_TYPE type, char* instance, const char* data)
    {
        switch(type)
        {
            case TYPE_BYTE:
                return deserialize<char>(ctx, instance, data);
            case TYPE_SHORT:
                return deserialize<uint16_t>(ctx, (uint16_t*)instance, data);
            case TYPE_INT:
                return deserialize<uint32_t>(ctx, (uint32_t*)instance, data);
            case TYPE_LONG:
                return deserialize<uint64_t>(ctx, (uint64_t*)instance, data);
            
            case TYPE_FLOAT:
                return deserialize<float>(ctx, (float*)instance, data);
            case TYPE_DOUBLE:
                return deserialize<double>(ctx, (double*)instance, data);

            case TYPE_STRING:
                return deserialize<String>(ctx, (String*)instance, data);
            
            // Reading objects requires an additional parameter.
            // We do not possess this parameter.
            case TYPE_OBJECT:
                if( referenced == nullptr )
                {
                    throw std::logic_error("Attribute type is BLF_OBJECT yet the referenced object is null.");
                }

                return referenced->deserialize(ctx, instance, data);

            case TYPE_OBJECTREFERENCE:
                // TODO: implement
                throw std::logic_error("Not implemented");
                return;
                
            case TYPE_UNKNOWN:
            case TYPE_NULL: 
                return;
        }
    }
}