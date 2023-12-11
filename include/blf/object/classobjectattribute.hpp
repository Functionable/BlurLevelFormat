#pragma once

#include "objectattribute.hpp"

namespace blf
{
    template<typename Class>
    class ClassObjectAttribute : public ObjectAttribute
    {
        public:
            ClassObjectAttribute(const String& name, const BLF_TYPE type)
                : ObjectAttribute(name, type)
            {}

            virtual ~ClassObjectAttribute() override = default;

            virtual void deserialize(SerializationContext& ctx, Class* instance, const char* sourceData) const = 0;
            virtual size_t measureSpan(SerializationContext& ctx, const char* data) const = 0;
            
            virtual void serialize(SerializationContext& ctx, const Class* instance, char* destinationData) const = 0;
            virtual size_t calculateSpan(SerializationContext& ctx, const Class* instance) const = 0;
    };
}