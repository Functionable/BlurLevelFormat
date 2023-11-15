#pragma once

#include "objectdefinition.hpp"
#include "classobjectattribute.hpp"

#include "../serialization/dataconversion.hpp"

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

            /*size_t getSize() const override
            {
                size_t size = 0;
                for( const ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    size += attribute->getSize();
                }

                return size;
            }*/

            void deserialize(T* instance, const char* data) const
            {
                const char* head = data;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    attribute->deserialize(instance, head);
                    //head += attribute->getSize();
                    head += attribute->measureSpan(head);
                }
            }

            /**
             * From a given serialized buffer of data, where we know that the
             * object is located at the beginning, the size of the object in
             * bytes will be measured.
             */
            size_t measureDataSpan(const char* data) const
            {
                size_t size = 0;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    size += attribute->measureSpan(data + size);
                }
                return size;
            }

            void deserialize(void* instance, const char* data) const override
            {
                return deserialize((T*)instance, data);
            }

            void serialize(const T* instance, char* data) const
            {
                char* head = data;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    attribute->serialize(instance, head);
                    head += attribute->calculateSpan(instance);
                }
            }

            /**
             * Calculates the number of bytes the given object will span when
             * serialized as bytes.
             */
            size_t calculateDataSpan(const T* instance) const
            {
                size_t size = 0;
                for( ClassObjectAttribute<T>* attribute : m_attributes )
                {
                    size += attribute->calculateSpan(instance);
                }
                return size;
            }
    };
}