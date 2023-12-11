#pragma once

#include "../string.hpp"

#include "../serialization/serializationcontext.hpp"

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

            virtual ~ObjectDefinition() = default;

            const String& getName() const { return m_name; }

            virtual void deserialize(SerializationContext& ctx, char* instance, const char* data) const = 0;
            virtual void serialize(SerializationContext& ctx, const char* data, char* destination) const = 0;
            virtual size_t measureSpan(SerializationContext& ctx, const char* data) const = 0;
            virtual size_t calculateSpan(SerializationContext& ctx, const char* instance) const = 0;
    };
}