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

            void deserialize(Class* instance, const char* sourceData) const override
            {
                //T* argPtr = (T*)sourceData;
                //(instance->*m_ptr) = *argPtr;
                //blf::deserialize(&(instance->*m_ptr), sourceData);
                //T* objectInstance = m_locationStore.ptr(instance);//&(instance->*m_ptr);
                /*if constexpr (!is_class_v<T>)
                {
                    blf::deserialize(objectInstance, sourceData);
                }
                else 
                {
                    m_definition->deserialize(objectInstance, sourceData);
                }*/
                //blf::deserialize(T *instance, const char *data)
                //m_procedureStore.deserialize(objectInstance, sourceData);

                if constexpr (!AttributeLocation::forceCopy)
                {
                    T* objectInstance = m_locationStore.ptr(instance);
                    m_procedureStore.deserialize(objectInstance, sourceData);
                }
                else 
                {
                    T objectInstance = m_locationStore.get(instance);
                    m_procedureStore.deserialize(&objectInstance, sourceData);
                    m_locationStore.set(instance, objectInstance);
                }
            }

            size_t measureSpan(const char* data) const override
            {
                /*if constexpr (!is_class_v<T>)
                {
                    return blf::measureData<T>(data);
                }
                else 
                {
                    return m_definition->measureDataSpan(data);
                }*/
                return m_procedureStore.measureSpan(data);
            }

            void serialize(const Class* instance, char* destinationData) const override
            {
                //T* argPtr = (T*)destinationData;
                //*argPtr = (instance->*m_ptr);
                /*T* const objectInstance = &(instance->*m_ptr);
                if constexpr (!is_class_v<T>)
                {
                    blf::serialize(objectInstance, destinationData);
                }
                else 
                {
                    m_definition->serialize(objectInstance, destinationData);
                }*/
                //const T* objectInstance = m_locationStore.ptr(instance);
                //m_procedureStore.serialize(objectInstance, destinationData);

                if constexpr( !AttributeLocation::forceCopy )
                {
                    const T* objectInstance = m_locationStore.ptr(instance);
                    m_procedureStore.serialize(objectInstance, destinationData);
                }
                else 
                {
                    const T objectInstance = m_locationStore.get(instance);
                    m_procedureStore.serialize(&objectInstance, destinationData);
                }
            }

            size_t calculateSpan(const Class* instance) const override
            {
                /*T* objectPtr = &(instance->*m_ptr);
                if constexpr(!is_class_v<T>)
                {
                    return blf::measure(objectPtr);
                }
                else 
                {
                    return m_definition->calculateDataSpan(objectPtr);
                }*/

                if constexpr ( !AttributeLocation::forceCopy )
                {
                    const T* objectInstance = m_locationStore.ptr(instance);
                    return m_procedureStore.calculateSpan(objectInstance);
                }
                else 
                {
                    const T objectInstance = m_locationStore.get(instance);
                    return m_procedureStore.calculateSpan(&objectInstance);
                }
            }
    };
}