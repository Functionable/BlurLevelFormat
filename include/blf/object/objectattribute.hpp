#pragma once

#include "objectdefinition.hpp"

#include "../types.hpp"
#include "../enums.hpp"
#include "../string.hpp"

#include <cstdint>

namespace blf
{
    class ObjectAttribute
    {
        private:
            String m_name;
            BLF_TYPE m_type;
            const ObjectDefinition* m_referencedObject;

        public:
            ObjectAttribute(const String& name="", 
                const BLF_TYPE type=TYPE_NULL, 
                const ObjectDefinition* referencedObject = nullptr)
                : m_name(name), m_type(type),
                  m_referencedObject(referencedObject)
            {}

            virtual ~ObjectAttribute() = default;

            const ObjectDefinition* getReferencedObjectName() const { return m_referencedObject; }
            const int8_t getSize() const { return typeSize(m_type); }
            const String& getName() const { return m_name; }
            const BLF_TYPE getType() const { return m_type; }
    };
}