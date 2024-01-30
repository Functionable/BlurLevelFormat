#pragma once

#include "blf/serialization/dataconversion.hpp"

namespace blf
{
    void serialize(SerializationContext& ctx, 
            const BLF_TYPE type, const char* data, char* location)
    {
        ;
    }   

    int dataLength(SerializationContext& ctx,
            const ObjectDefinition* referenced, const BLF_TYPE type, 
            const char* data)
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
                return dataLength<String>(ctx, data);
            case TYPE_OBJECT:
                return referenced->dataLength(ctx, data);
            case TYPE_COMPAT_OBJECTREFERENCE:
                throw std::logic_error("Not supported in >=2.0.0.");
                return -1;

            case TYPE_NULL:
            case TYPE_UNKNOWN:
                return 0;
        }

        return -1;
    }
    
    void deserialize(SerializationContext& ctx, 
            const ObjectDefinition* referenced, const BLF_TYPE type, 
            char* instance, const char* data)
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

            case TYPE_COMPAT_OBJECTREFERENCE:
                // TODO: implement
                throw std::logic_error("Not supported in >=blf-2.0.0");
                return;
                
            case TYPE_UNKNOWN:
            case TYPE_NULL: 
                return;
        }
    }
}