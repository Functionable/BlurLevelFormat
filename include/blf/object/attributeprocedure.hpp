#pragma once

#include "classobjectattribute.hpp"
#include "localobjectdefinition.hpp"
#include "../serialization/dataconversion.hpp"

#include <type_traits>

namespace blf
{
    template<typename Class, typename T>
    struct GenericAttributeProcedure
    {
        struct Store
        {
            inline constexpr void deserialize(T* instance, const char* data) const
            {
                blf::deserialize(instance, data);
            }

            inline constexpr void serialize(const T* objectInstance, char* destinationData) const
            {
                blf::serialize(objectInstance, destinationData);
            }

            inline constexpr size_t measureSpan(const char* data) const
            {
                return blf::measureData<T>(data);
            }
            
            inline constexpr size_t calculateSpan(const T* object) const
            {
                return blf::measure(object);
            }
        };
    };

    template<typename Class, typename T>
    struct ObjectAttributeProcedure
    {
        struct Store
        {
            const LocalObjectDefinition<T>* definition;

            inline constexpr void deserialize(T* instance, const char* data) const
            {
                definition->deserialize(instance, data);
            }

            inline constexpr void serialize(const T* objectInstance, char* destinationData) const
            {
                definition->serialize(objectInstance, destinationData);
            }

            inline constexpr size_t measureSpan(const char* data) const
            {
                return definition->measureDataSpan(data);
            }
            
            inline constexpr size_t calculateSpan(const T* object) const
            {
                return definition->calculateDataSpan(object);
            }
        };
    };

    template<typename T>
    struct is_attribute_procedure : std::false_type {};

    template<typename Class, typename T>
    struct is_attribute_procedure<GenericAttributeProcedure<Class, T>> : std::true_type {};

    template<typename Class, typename T>
    struct is_attribute_procedure<ObjectAttributeProcedure<Class, T>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_attribute_procedure_v = is_attribute_procedure<T>::value;
}