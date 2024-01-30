#pragma once

#include "blf/serialization/dataconversion.hpp"
#include "classobjectattribute.hpp"
#include "localobjectdefinition.hpp"

#include "attributelocation.hpp"
#include "attributeprocedure.hpp"

namespace blf
{
    template<typename Class, typename T, typename AttributeLocation,
        typename AttributeProcedure,
        typename = std::enable_if_t<is_attribute_location_v<AttributeLocation>
                                && is_attribute_procedure_v<AttributeProcedure>>>
    class SpecializedClassObjectAttribute : public ClassObjectAttribute<Class>
    {
        private:
            typename AttributeLocation::Store m_locationStore;
            typename AttributeProcedure::Store m_procedureStore;

        public:
            SpecializedClassObjectAttribute(const String& name, BLF_TYPE type,
                typename AttributeLocation::Store locationStore,
                typename AttributeProcedure::Store procedureStore)
                : ClassObjectAttribute<Class>(name, type), 
                    m_locationStore(locationStore), m_procedureStore(procedureStore)
            {}

            ~SpecializedClassObjectAttribute() override = default;

            // TODO: Remove the repetition of ::forceCopy in these methods.
            //       Surely there's a better way to do this!
            void deserialize(SerializationContext& ctx, Class* instance, const char* sourceData) const override
            {
                if constexpr (!AttributeLocation::forceCopy)
                {
                    T* objectInstance = m_locationStore.ptr(instance);
                    m_procedureStore.deserialize(ctx, objectInstance, sourceData);
                }
                else 
                {
                    auto objectInstance = m_locationStore.get(instance);
                    m_procedureStore.deserialize(ctx, &objectInstance, sourceData);
                    m_locationStore.set(instance, objectInstance);
                }
            }

            size_t dataLength(SerializationContext& ctx, const char* data) const override
            {
                return m_procedureStore.dataLength(ctx, data);
            }

            void serialize(SerializationContext& ctx, const Class* instance, char* destinationData) const override
            {
                if constexpr( !AttributeLocation::forceCopy )
                {
                    const T* objectInstance = m_locationStore.ptr(instance);
                    m_procedureStore.serialize(ctx, objectInstance, destinationData);
                }
                else 
                {
                    auto objectInstance = m_locationStore.get(instance);
                    m_procedureStore.serialize(ctx, &objectInstance, destinationData);
                }
            }

            size_t serializedLength(SerializationContext& ctx, const Class* instance) const override
            {
                if constexpr ( !AttributeLocation::forceCopy )
                {
                    const T* objectInstance = m_locationStore.ptr(instance);
                    return m_procedureStore.serializedLength(ctx, objectInstance);
                }
                else 
                {
                    auto objectInstance = m_locationStore.get(instance);
                    return m_procedureStore.serializedLength(ctx, &objectInstance);
                }
            }
    };
}