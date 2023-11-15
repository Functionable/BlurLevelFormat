#pragma once

#include "objectattribute.hpp"

#include "../string.hpp"
#include "../bakedlist.hpp"

#include <cstddef>

namespace blf
{
    class ObjectDefinition
    {
        private:
            String m_name;

        public:
            ObjectDefinition(const String& name)
                : m_name(name)
            {}

            virtual ~ObjectDefinition() {}

            const String& getName() const { return m_name; }

            virtual void deserialize(void* instance, const char* data) const
            {
                
            }

            /**
             * Returns DYNAMIC_TYPE if the size can change, whether it is because of
             * the inclusion of a String type or another object within.
             */
            // TODO: probably fully remove? seems useless now that calculateSpan is a thing.
            //virtual size_t getSize() const = 0;
    };
}