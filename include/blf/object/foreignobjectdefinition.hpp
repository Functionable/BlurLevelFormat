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
            ForeignObjectDefinition() :
                m_attributes(UnbakedList<ObjectAttribute>()), 
                ObjectDefinition(NULL_OBJECT_NAME) 
            {}

            ForeignObjectDefinition(const String& name, 
                const BakedList<ObjectAttribute>& attributes)
                : m_attributes(attributes), ObjectDefinition(name)
            {}

            ForeignObjectDefinition(const ForeignObjectDefinition& definition)
                : m_attributes(definition.m_attributes),
                ObjectDefinition(definition.getName())
            {}

            ~ForeignObjectDefinition() override = default;

            const BakedList<ObjectAttribute>& getAttributes() const { return m_attributes; }
            void setAttributes(const BakedList<ObjectAttribute>& attributes) { m_attributes = attributes; }

            void deserialize(SerializationContext& ctx, char* instance, const char* data) const override;
            void serialize(SerializationContext& ctx, const char* data, char* destination) const override;

            virtual size_t dataLength(SerializationContext& ctx, const char* data) const override;
            virtual size_t serializedLength(SerializationContext& ctx, const char* instance) const override;
    };
}