#pragma once

#include "objectdefinition.hpp"

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

            // TODO: this method.
            /*size_t getSize() const
            {
                size_t size = 0;
                for( const ObjectAttribute& attribute : m_attributes )
                {
                    size += attribute.getSize();
                }

                return size;
            }*/
    };
}