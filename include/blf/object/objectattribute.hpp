#pragma once

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
            ObjectID m_referencedObject;

        public:
            ObjectAttribute(const String& name="", 
                const BLF_TYPE type=TYPE_NULL, 
                const ObjectID referencedObject = NULL_OBJECT)
                : m_name(name), m_type(type),
                  m_referencedObject(referencedObject)
            {}

            virtual ~ObjectAttribute() {}

            const ObjectID getReferencedObjectID() const { return m_referencedObject; }
            const String& getName() const { return m_name; }
            const uint8_t getType() const { return m_type; }
            const int8_t  getSize() const { return typeSize(m_type); }
    };
}