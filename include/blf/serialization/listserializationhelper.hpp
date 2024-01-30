#pragma once

#include "../enums.hpp"
#include "../types.hpp"

#include "../object/objectdefinition.hpp"
#include "serializationcontext.hpp"

#include <type_traits>
#include <vector>

namespace blf
{
    
    template<typename T, typename ItemStore, typename = std::enable_if_t<is_list_v<T>>>
    struct ListSerializationHelper
    {
        inline static listsize_t size(const T* data);

        inline static int serializedLength(SerializationContext& context, 
            const ItemStore& store, const T* data);
        inline static int serializeItem(SerializationContext& context, 
            const ItemStore& store, const T* data, 
            listsize_t index, char* location);

        inline static T create(/* ListType* array, listsize_t size */);

    };

    struct ListItemSerializationHelper
    {
        template<typename T, typename ItemStore, typename Result>
        inline static void deserializeList(SerializationContext& ctx, 
            const ItemStore& store, Result* instance, 
            const char* data)
        {
            const char* head = data;
            listsize_t size = *(listsize_t*)(data);

            head += sizeof(listsize_t);

            T* temporaryList = new T[size];

            for(listsize_t i = 0; i < size; i++)
            {
                store.deserialize(ctx, &temporaryList[i], head);
                head += store.dataLength(ctx, data);
            }

            *instance = ListSerializationHelper<Result, ItemStore>::create(temporaryList, size);

            delete[] temporaryList;
        }
    };

    template<typename T, typename ItemStore>
    struct ListSerializationHelper<std::vector<T>, ItemStore>
    {
        inline static int size(const std::vector<T>* data)
        {
            return data->size();
        }

        inline static int serializedLength(SerializationContext& context,
            const ItemStore& store, const std::vector<T>* data)
        {
            int size = 0;
            for(const T& value : *data)
            {
                size += store.serializedLength(context, &value);
            }

            return size;
        }

        inline static void deserialize(SerializationContext& ctx, 
            const ItemStore& store, std::vector<T>* instance, const char* data)
        {
            return ListItemSerializationHelper::deserializeList<T, ItemStore, std::vector<T>>(
                ctx, store, instance, data
            );
        }

        inline static int serializeItem(SerializationContext& context, 
            const ItemStore& store, const std::vector<T>* data, 
            listsize_t index, char* location)
        {
            const T& instance = data->at(index);
            store.serialize(context, &instance, location);
            return store.serializedLength(context, &instance);
        }

        inline static std::vector<T> create(T* array, listsize_t size)
        {
            return std::vector<T>(array, array+size);
        }
    };

    template<typename T, typename ItemStore>
    struct ListSerializationHelper<BakedList<T>, ItemStore>
    {
        inline static int size(const BakedList<T>* list)
        {
            return list->size();
        }

        inline static int serializedLength(SerializationContext& context,
            const ItemStore& store, const BakedList<T>* data)
        {
            int size = 0;
            for(const T& value : *data)
            {
                size += store.serializedLength(context, &value);
            }

            return size;
        } 

        inline static void deserialize(SerializationContext& ctx, 
            const ItemStore& store, BakedList<T>* instance, const char* data)
        {
            return ListItemSerializationHelper::deserializeList<T, ItemStore, BakedList<T>>(
                ctx, store, instance, data
            );
        }

        inline static int serializeItem(SerializationContext& context, 
            const ItemStore& store, const BakedList<T>* data, 
            listsize_t index, char* location)
        {
            const T& instance = data->get(index);
            store.serialize(context, &instance, location);
            return store.serializedLength(context, &instance);
        }

        inline static BakedList<T> create(T* array, listsize_t size)
        {
            return UnbakedList<T>(array, size).bake();
        }
    };

    template<typename T, typename ItemStore>
    struct ListSerializer
    {
        private:
            using ItemType = typename list_traits<T>::item_type;

        public:
            inline static void serialize(SerializationContext& ctx, 
                const ItemStore& store, const T* data, char* location)
            {
                listsize_t listSize = ListSerializationHelper<T, ItemStore>::size(data);
                *(listsize_t*)(location) = listSize;

                char* start = location + sizeof(listsize_t);

                for( listsize_t i = 0; i < listSize; i++ )
                {
                    start += ListSerializationHelper<T, ItemStore>::serializeItem(ctx, store, 
                                                                data, i, start);
                }
            }

            inline static void deserialize(SerializationContext& ctx, 
                const ItemStore& store, T* instance, const char* data)
            {
                ListSerializationHelper<T, ItemStore>::deserialize(ctx, store, instance, data);
            }

            inline static int serializedLength(SerializationContext& ctx, 
                const ItemStore& store, const T* data)
            {
                return sizeof(listsize_t) + 
                    ListSerializationHelper<T, ItemStore>::serializedLength(ctx,  store, data);
            }

            inline static int dataLength(SerializationContext& ctx, 
                const ItemStore& store, const char* data)
            {
                listsize_t size = *(listsize_t*)(data);

                const char* head = data + sizeof(listsize_t);

                for(listsize_t i = 0; i < size; i++)
                {
                    head += store.dataLength(ctx, head);
                }

                return (head-data);
            }
    };
}