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
            // used if m_type == BLF_OBJECT.
            ObjectID m_object;

        public:
            ObjectAttribute(const String& name="", 
                const BLF_TYPE type=TYPE_NULL, 
                ObjectID object = NULL_OBJECT)
                : m_name(name), m_type(type),
                  m_object(object)
            {}

            virtual ~ObjectAttribute() = default;

            ObjectID getObject() const { return m_object; }
            int8_t getSize() const { return typeSize(m_type); }
            const String& getName() const { return m_name; }
            BLF_TYPE getType() const { return m_type; }

            void setName(const String& name) { m_name = name; }
            void setType(BLF_TYPE type) { m_type = type; }
            void setID(ObjectID id) {m_object = id;}
    };
}