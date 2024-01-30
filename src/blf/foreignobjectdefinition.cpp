#include "blf/object/foreignobjectdefinition.hpp"

#include "blf/serialization/dataconversion.hpp"

namespace blf
{
    void ForeignObjectDefinition::deserialize(SerializationContext& ctx, char* instance, const char* data) const
    {
        const char* head = data;
        char* whead = (char*)instance;
        for(ObjectAttribute attribute : m_attributes )
        {
            const BLF_TYPE type = attribute.getType();
            const size_t offset = blf::dataLength(ctx, this, type, head);
            blf::deserialize(ctx, this, type, whead, head);
            whead += offset;
            head += offset;
        }
    }

    void ForeignObjectDefinition::serialize(SerializationContext& ctx, const char* data, char* destination) const
    {
        return;
    }

    size_t ForeignObjectDefinition::dataLength(SerializationContext& ctx, const char* data) const
    {
        return -1;
    }

    size_t ForeignObjectDefinition::serializedLength(SerializationContext& ctx, const char* instance) const
    {
        return -1;
    }
    
}
