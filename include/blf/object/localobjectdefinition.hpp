#pragma once

#include "objectdefinition.hpp"
#include "classobjectattribute.hpp"

#include "../serialization/dataconversion.hpp"
#include "../serialization/serializationcontext.hpp"

namespace blf
{
    template<typename T>
    class LocalObjectDefinition : public ObjectDefinition
    {
        private:
            BakedList<ClassObjectAttribute<T>*> m_attributes;

        public:
            LocalObjectDefinition(const String& name, 
                const BakedList<ClassObjectAttribute<T>*>& attributes)
                : m_attributes(attributes), ObjectDefinition(name)
            {}

            ~LocalObjectDefinition() override = default; 

            void deserialize(SerializationContext& ctx, T* instance, const char* data) const
            {
                const char* head = data;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    attribute->deserialize(ctx, instance, head);
                    head += attribute->measureSpan(ctx, head);
                }
            }

            /**
             * From a given serialized buffer of data, where we know that the
             * object is located at the beginning, the size of the object in
             * bytes will be measured.
             */
            size_t measureSpan(SerializationContext& ctx, const char* data) const override
            {
                size_t size = 0;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    size += attribute->measureSpan(ctx, data + size);
                }
                return size;
            }

            void deserialize(SerializationContext& ctx, char* instance, const char* data) const override
            {
                return deserialize(ctx, (T*)instance, data);
            }

            void serialize(SerializationContext& ctx, const T* instance, char* data) const
            {
                char* head = data;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    attribute->serialize(ctx, instance, head);
                    head += attribute->calculateSpan(ctx, instance);
                }
            }

            void serialize(SerializationContext& ctx, const char* data, char* destination) const override
            {
                serialize(ctx, (T*)data, destination);
            }

            /**
             * Calculates the number of bytes the given object will span when
             * serialized as bytes.
             */
            size_t calculateDataSpan(SerializationContext& ctx, const T* instance) const
            {
                size_t size = 0;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    size += attribute->calculateSpan(ctx, instance);
                }
                return size;
            }


            size_t calculateSpan(SerializationContext& ctx, const char* instance) const override
            {
                return calculateDataSpan(ctx, (const T*)instance);
            }
    };
}