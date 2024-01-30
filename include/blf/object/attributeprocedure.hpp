#pragma once

#include "classobjectattribute.hpp"
#include "localobjectdefinition.hpp"

#include "../serialization/dataconversion.hpp"
#include "../serialization/listserializationhelper.hpp"

#include <type_traits>

namespace blf
{
    template<typename T>
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

            inline constexpr size_t dataLength(SerializationContext& ctx, const char* data) const
            {
                return blf::dataLength<T>(ctx, data);
            }
            
            inline constexpr size_t serializedLength(SerializationContext& ctx, const T* object) const
            {
                return blf::serializedLength(ctx, object);
            }

            Store() = default;
        };
    };

    template<typename T>
    struct ObjectAttributeProcedure
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

            inline constexpr size_t dataLength(SerializationContext& ctx, const char* data) const
            {
                return definition->dataLength(ctx, data);
            }
            
            inline constexpr size_t serializedLength(SerializationContext& ctx, const T* object) const
            {
                return definition->serializedLength(ctx, object);
            }

            Store(const LocalObjectDefinition<T>& def)
            {
                definition = &def;
            }

            Store(const LocalObjectDefinition<T>* def)
            {
                definition = def;
            }
        };
    };

    template<typename T, typename Enable = void>
    struct infer_attribute_procedure 
    {
        using type = GenericAttributeProcedure<T>;
    };

    template<typename T>
    struct ListAttributeProcedure
    {
        private:
            using ItemType = typename list_traits<T>::item_type;
            using ItemProcedure = typename infer_attribute_procedure<ItemType>::type;
            using ItemStore = typename ItemProcedure::Store;
            struct ListStore : public ItemStore
            {
                using typename ItemStore::Store;

                inline constexpr void deserialize(SerializationContext& ctx, T* instance, const char* data) const
                {
                    //definition->deserialize(ctx, instance, data);
                    ListSerializer<T, ItemStore>::deserialize(ctx, *this, instance, data);
                }

                inline constexpr void serialize(SerializationContext& ctx, const T* objectInstance, char* destinationData) const
                {
                    ListSerializer<T,ItemStore>::serialize(ctx, *this, objectInstance, destinationData);
                    //definition->serialize(ctx, objectInstance, destinationData);
                }

                inline constexpr size_t dataLength(SerializationContext& ctx, const char* data) const
                {
                    return ListSerializer<T, ItemStore>::dataLength(ctx, *this, data);
                    //return definition->dataLength(ctx, data);
                }
                
                inline constexpr size_t serializedLength(SerializationContext& ctx, const T* object) const
                {
                    return ListSerializer<T, ItemStore>::serializedLength(ctx, *this, object);
                    //return definition->serializedLength(ctx, object);
                }
            };

        public:
            using Store=ListStore;
    };

    template<typename T>
    struct is_attribute_procedure : std::false_type {};

    template<typename T>
    struct is_attribute_procedure<GenericAttributeProcedure<T>> : std::true_type {};

    template<typename T>
    struct is_attribute_procedure<ObjectAttributeProcedure<T>> : std::true_type {};

    template<typename T>
    struct is_attribute_procedure<ListAttributeProcedure<T>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_attribute_procedure_v = is_attribute_procedure<T>::value;

    template<typename T>
    struct infer_attribute_procedure<T, std::enable_if_t<is_class_v<T>>>
    {
        using type = ObjectAttributeProcedure<T>;
    };

    template<typename T>
    struct infer_attribute_procedure<T, std::enable_if_t<is_list_v<T>>>
    {
        using type = ListAttributeProcedure<T>;
    };

    template<typename T>
    using infer_attribute_procedure_t = typename infer_attribute_procedure<T>::type;
}