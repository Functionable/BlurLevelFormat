#pragma once

#include "objectattribute.hpp"
#include "objectdefinition.hpp"

#include "../bakedlist.hpp"

namespace blf
{
    class ForeignObjectDefinition : public ObjectDefinition
    {
        private:
            BakedList<ObjectAttribute> m_attributes;

        public:
            ForeignObjectDefinition(const String& name, 
                const BakedList<ObjectAttribute>& attributes)
                : m_attributes(attributes), ObjectDefinition(name)
            {}

            ~ForeignObjectDefinition() override = default;

            void deserialize(SerializationContext& ctx, char* instance, const char* data) const override;
            void serialize(SerializationContext& ctx, const char* data, char* destination) const override;

            virtual size_t measureSpan(SerializationContext& ctx, const char* data) const override;
            virtual size_t calculateSpan(SerializationContext& ctx, const char* instance) const override;
    };
}