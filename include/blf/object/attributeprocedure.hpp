#pragma once

#include "classobjectattribute.hpp"
#include "localobjectdefinition.hpp"

#include "../commontable.hpp"
#include "../serialization/dataconversion.hpp"

#include <type_traits>

namespace blf
{
    template<typename Class, typename T>
    struct GenericAttributeProcedure
    {
        struct Store
        {
            inline constexpr void deserialize(SerializationContext& ctx, T* instance, const char* data) const
            {
                blf::deserialize(ctx, instance, data);
            }

            inline constexpr void serialize(SerializationContext& ctx, const T* objectInstance, char* destinationData) const
            {
                blf::serialize(ctx, objectInstance, destinationData);
            }

            inline constexpr size_t measureSpan(SerializationContext& ctx, const char* data) const
            {
                return blf::measureData<T>(ctx, data);
            }
            
            inline constexpr size_t calculateSpan(SerializationContext& ctx, const T* object) const
            {
                return blf::measure(ctx, object);
            }
        };
    };

    template<typename Class, typename T>
    struct ObjectAttributeProcedure
    {
        struct Store
        {
            const LocalObjectDefinition<std::decay_t<T>>* definition;

            inline constexpr void deserialize(SerializationContext& ctx, T* instance, const char* data) const
            {
                definition->deserialize(ctx, instance, data);
            }

            inline constexpr void serialize(SerializationContext& ctx, const T* objectInstance, char* destinationData) const
            {
                definition->serialize(ctx, objectInstance, destinationData);
            }

            inline constexpr size_t measureSpan(SerializationContext& ctx, const char* data) const
            {
                return definition->measureSpan(ctx, data);
            }
            
            inline constexpr size_t calculateSpan(SerializationContext& ctx, const T* object) const
            {
                return definition->calculateDataSpan(ctx, object);
            }
        };
    };

    template<typename Class, typename T>
    struct ReferenceAttributeProcedure
    {
        struct Store
        {
            const LocalObjectDefinition<T>* definition;

            inline constexpr void deserialize(SerializationContext& ctx, T* instance, const char* data) const
            {
                definition->deserialize(ctx, instance, data);
            }

            inline constexpr void serialize(SerializationContext& ctx, const T* objectInstance, char* destinationData) const
            {
                definition->serialize(ctx, objectInstance, destinationData);
            }

            inline constexpr size_t measureSpan(SerializationContext& ctx, const char* data) const
            {
                return definition->measureSpan(ctx, data);
            }
            
            inline constexpr size_t calculateSpan(SerializationContext& ctx, const T* object) const
            {
                return definition->calculateDataSpan(ctx, object);
            }
        };
    };

    template<typename T>
    struct CommonAttributeProcedure
    {
        struct Store
        {
            CommonTable* table;
        };
    };

    template<typename T>
    struct is_attribute_procedure : std::false_type {};

    template<typename Class, typename T>
    struct is_attribute_procedure<GenericAttributeProcedure<Class, T>> : std::true_type {};

    template<typename Class, typename T>
    struct is_attribute_procedure<ObjectAttributeProcedure<Class, T>> : std::true_type {};

    template<typename Class, typename T>
    struct is_attribute_procedure<ReferenceAttributeProcedure<Class, T>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_attribute_procedure_v = is_attribute_procedure<T>::value;
}