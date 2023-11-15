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

            virtual ~ClassObjectAttribute() {}

            virtual void deserialize(Class* instance, const char* sourceData) const = 0;
            virtual size_t measureSpan(const char* data) const = 0;
            
            virtual void serialize(const Class* instance, char* destinationData) const = 0;
            virtual size_t calculateSpan(const Class* instance) const = 0;
    };
}