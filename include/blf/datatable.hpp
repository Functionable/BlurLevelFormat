#pragma once

#include <cstddef>
#include <utility>

#include "bakedlist.hpp"
#include "objectdata.hpp"
#include "unbakedlist.hpp"

#include "commontable.hpp"
#include "localobjecttable.hpp"

#include "object/objectdefinition.hpp"

namespace blf
{
    using UnbakedDataList = UnbakedList<ObjectData>;

    struct ObjectView
    {
        void* data;
        ObjectDefinition* definition;
    };

    class DataTable
    {
        private:
            BakedList<ObjectData> m_objectViews;

        public:
            DataTable(const BakedList<ObjectData>& data)
                : m_objectViews(data)
            {}

            DataTable(const DataTable& table)
                : m_objectViews(table.m_objectViews)
            {}

            DataTable(DataTable&& table)
                : m_objectViews(std::move(table.m_objectViews))
            {}

            size_t size() const { return m_objectViews.size(); }
    };

    class UnbakedDataTable
    {
        private:
            std::vector<ObjectView> m_views;

        public:
            UnbakedDataTable()
                : m_views()
            {}

            UnbakedDataTable(const UnbakedDataTable& table)
                : m_views(table.m_views)
            {}

            template<typename T>
            void add(const LocalObjectDefinition<T>& definition, T& value)
            {
                //ObjectView newObjectView = {&value, definition};

                //m_views.push_back(newObjectView);
                m_views.emplace_back(&value, definition);
            }

            auto begin() { return m_views.begin(); }
            auto end() { return m_views.end(); }

            /*DataTable bake()
            {
                UnbakedList<ObjectData> list;
                for(const ObjectView& view : m_views)
                {
                    view.definition->calculateSpan(view.data);
                }
            }*/

            size_t size() const { return m_views.size(); }
    }
}