#pragma once

#include "types.hpp"
#include "objectdata.hpp"

namespace blf
{
    /**
     * A version of ObjectData for use with the DataTable, which
     * contains a reference to the ObjectID of the data stored in the object.
     */
    class NamedObjectData : public ObjectData
    {
        private:
            ObjectID m_ID;

        public:
            NamedObjectData(ObjectID ID, char* data, size_t size)
                : ObjectData(data, size), m_ID(ID)
            {}

            NamedObjectData(ObjectID ID, const ObjectData& data)
                : ObjectData(data), m_ID(ID)
            {}

            NamedObjectData(ObjectID ID, ObjectData&& data)
                : ObjectData(data), m_ID(ID)
            {}

            NamedObjectData(const NamedObjectData& data)
                : ObjectData(data), m_ID(data.m_ID)
            {}

            NamedObjectData(NamedObjectData&& data)
                : ObjectData(data), m_ID(std::move(data.m_ID))
            {}

            ~NamedObjectData() = default;

            /**
             * Returns the ObjectID corresponding to the data stored.
             */
            ObjectID getObject() const { return m_ID; }
    };
}